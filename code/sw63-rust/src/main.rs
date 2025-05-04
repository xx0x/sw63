#![no_std]
#![no_main]

mod ds3231;

use atsamd_hal::{
    adc::{Adc, Gain, Reference, Resolution, SampleRate},
    clock::GenericClockController,
    delay::Delay,
    gpio::v2::{Pins, B, C, Pin, PullUpInterrupt, PA04},
    prelude::*,
    pac::{interrupt, EIC, NVIC},
    sercom::i2c,
    target_device::Peripherals,
    time::U32Ext,
};
use cortex_m::interrupt::Mutex;
use cortex_m_rt::entry;
use panic_halt as _;
use core::cell::RefCell;
use core::sync::atomic::{AtomicBool, Ordering};

use ds3231::{Time, DS3231};

// Global variables for button state
static BUTTON_PRESSED: AtomicBool = AtomicBool::new(false);
static G_BUTTON: Mutex<RefCell<Option<Pin<PA04, PullUpInterrupt>>>> = Mutex::new(RefCell::new(None));

#[entry]
fn main() -> ! {
    // Get peripherals
    let mut peripherals = Peripherals::take().unwrap();
    let core = cortex_m::Peripherals::take().unwrap();

    // Configure clocks using the internal oscillator
    let mut clocks = GenericClockController::with_internal_32kosc(
        peripherals.GCLK,
        &mut peripherals.PM,
        &mut peripherals.SYSCTRL,
        &mut peripherals.NVMCTRL,
    );

    // Setup all clocks first to avoid multiple mutable borrows
    let gclk0 = clocks.gclk0();
    clocks.eic(&gclk0);
    let sercom0_core = clocks.sercom0_core(&gclk0).unwrap();
    
    // Setup delay provider
    let mut delay = Delay::new(core.SYST, &mut clocks);

    // Configure pins
    let pins = Pins::new(peripherals.PORT);

    // Configure LED on PA23 and button on PA04
    let mut led_pin = pins.pa23.into_push_pull_output();
    // Convert the button pin to pull-up interrupt mode
    let button_pin = pins.pa04.into_pull_up_interrupt();

    // Enable the EIC peripheral
    peripherals.PM.apbamask.modify(|_, w| w.eic_().set_bit());

    // Setup External Interrupt Controller (EIC)
    let eic = &peripherals.EIC;
    
    // Configure button interrupt (PA04 is EXTINT[4])
    // Configure the External Interrupt Controller
    unsafe {
        // Configure the event control register for EXTINT4
        // Set to trigger on falling edge (when button is pressed)
        eic.config[0].modify(|_, w| w.sense4().fall());
        
        // Enable the interrupt for EXTINT4
        eic.intenset.write(|w| w.extint4().set_bit());
        
        // Enable EIC
        eic.ctrl.write(|w| w.enable().set_bit());
        
        // Wait for synchronization
        while eic.status.read().syncbusy().bit_is_set() {}
    }

    // Store button in global variable for the interrupt handler
    cortex_m::interrupt::free(|cs| {
        G_BUTTON.borrow(cs).replace(Some(button_pin));
    });

    // Enable EIC interrupt in NVIC
    unsafe {
        NVIC::unmask(interrupt::EIC);
    }

    // Configure I2C pins (SDA on PA08, SCL on PA09)
    // For SERCOM0, both PA08 and PA09 need to be in Alternate C mode
    let sda = pins.pa08.into_alternate::<C>();
    let scl = pins.pa09.into_alternate::<C>();

    // Set up I2C - create a master port with the given pins
    let i2c = i2c::I2CMaster0::new(
        &sercom0_core,
        100.khz(),
        peripherals.SERCOM0,
        &mut peripherals.PM,
        sda,
        scl,
    );

    // Create DS3231 driver
    let mut rtc = DS3231::new(i2c);

    // Set the date and time to August 18, 2025, 04:20:00
    let time = Time {
        year: 2025,
        month: 8,
        day: 18,
        hour: 4,
        minute: 20,
        second: 0,
    };

    // Set the time on the DS3231
    if let Err(_) = rtc.set_time(&time) {
        // Handle error (just blink rapidly in this example)
        loop {
            led_pin.toggle().unwrap();
            delay.delay_ms(100u32);
        }
    }

    // Configure PA02 as an analog input
    let mut analog_pin = pins.pa02.into_alternate::<B>();

    // ADC
    let mut adc = Adc::adc(peripherals.ADC, &mut peripherals.PM, &mut clocks);
    adc.reference(Reference::INTVCC1); // Internal 1/2 VDDANA reference
    adc.resolution(Resolution::_12BIT); // 12-bit resolution
    adc.samples(SampleRate::_8); // 8 samples
    adc.gain(Gain::DIV2); // 1/2 gain

    // Blink count
    let mut blink_count = 0u32;

    // Main loop - blink LED and optionally read current time
    loop {
        // Check if button was pressed via interrupt
        if BUTTON_PRESSED.load(Ordering::Relaxed) {
            // Handle button press
            led_pin.toggle().unwrap();
            delay.delay_ms(50u32);
            led_pin.toggle().unwrap();
            
            // Reset the flag
            BUTTON_PRESSED.store(false, Ordering::Relaxed);
        }
        
        blink_count += 1u32;
        led_pin.toggle().unwrap();

        // Read time
        let _current_time = rtc.get_time().unwrap();

        // Read brightness from PA02
        let _brightness: u16 = adc.read(&mut analog_pin).unwrap();

        delay.delay_ms(250u32);

        // Fake deep sleep
        if blink_count >= 8 {
            led_pin.set_low().unwrap();
            blink_count = 0;
            delay.delay_ms(100u32);
            
            // Wait for interrupt to wake us up instead of polling
            while !BUTTON_PRESSED.load(Ordering::Relaxed) {
                // Low power waiting
                cortex_m::asm::wfi(); // Wait for interrupt
            }
            // Reset the flag
            BUTTON_PRESSED.store(false, Ordering::Relaxed);
        }
    }
}

#[interrupt]
fn EIC() {
    // Set the button pressed flag
    BUTTON_PRESSED.store(true, Ordering::Relaxed);
    
    // Clear the interrupt flag in the EIC
    unsafe {
        let eic = &(*EIC::ptr());
        // Clear interrupt for EXTINT4 (button on PA04)
        eic.intflag.modify(|_, w| w.extint4().set_bit());
    }
}