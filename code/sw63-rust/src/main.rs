#![no_std]
#![no_main]

mod ds3231;

use atsamd_hal::{
    adc::{Adc, Gain, Reference, Resolution, SampleRate},
    clock::GenericClockController,
    delay::Delay,
    gpio::v2::{Pins, B, C},
    pac::{interrupt, CorePeripherals, EIC, GCLK, NVIC},
    prelude::*,
    sercom::i2c,
    target_device::Peripherals,
    time::U32Ext,
};

use cortex_m_rt::entry;
use panic_halt as _;

use ds3231::{Time, DS3231};

#[entry]
fn main() -> ! {
    // Get peripherals
    let mut peripherals = Peripherals::take().unwrap();
    let mut core = CorePeripherals::take().unwrap();

    // Configure clocks using the internal oscillator
    let mut clocks = GenericClockController::with_internal_32kosc(
        peripherals.GCLK,
        &mut peripherals.PM,
        &mut peripherals.SYSCTRL,
        &mut peripherals.NVMCTRL,
    );

    // Configure pins
    let pins = Pins::new(peripherals.PORT);

    // Configure LED on PA23 and button on PA04
    let mut led_pin = pins.pa23.into_push_pull_output();

    // Setup delay provider
    let mut delay = Delay::new(core.SYST, &mut clocks);

    // Startup indicator
    led_pin.set_high().unwrap();
    delay.delay_ms(200u32);
    led_pin.set_low().unwrap();

    // ==================================================
    // Configure GCLK with minimal power for EIC operation
    // ==================================================

    let gclk = unsafe { &*GCLK::ptr() };

    // Configure GCLK1 to use OSC32K with minimal power
    unsafe {
        // First disable the clock generator
        gclk.genctrl.write(|w| w.id().bits(1).genen().clear_bit());
        while gclk.status.read().syncbusy().bit() {}

        // Set division factor to 32 (lower power)
        gclk.gendiv.write(|w| w.id().bits(1).div().bits(32));
        while gclk.status.read().syncbusy().bit() {}

        // Configure with OSC32K and enable in standby
        gclk.genctrl.write(|w| {
            w.id().bits(1)
             .src().osc32k()
             .idc().clear_bit()    // Disable improved duty cycle (lower power)
             .genen().set_bit()
             .runstdby().set_bit() // Must run in standby for wakeup to work
        });
        while gclk.status.read().syncbusy().bit() {}

        // Connect GCLK1 to the EIC
        gclk.clkctrl.write(|w| {
            w.id().eic()
             .gen().bits(1)
             .clken().set_bit()
        });
        while gclk.status.read().syncbusy().bit() {}
    }

    // Enable EIC peripheral clock
    peripherals.PM.apbamask.modify(|_, w| w.eic_().set_bit());

    // Get EIC access
    let eic = &peripherals.EIC;

    // Disable EIC before configuring
    eic.ctrl.write(|w| w.enable().clear_bit());
    while eic.status.read().syncbusy().bit_is_set() {}

    // Configure only the button interrupt (PA04 = EXTINT[4])
    // Use input filtering to avoid spurious wakeups
    eic.config[0].write(|w| w.sense4().fall().filten4().set_bit());

    // Only enable the one interrupt we need
    eic.intenset.write(|w| w.extint4().set_bit());

    // Only enable wakeup for the specific interrupt
    eic.wakeup.write(|w| w.wakeupen4().set_bit());

    // Enable EIC
    eic.ctrl.write(|w| w.enable().set_bit());
    while eic.status.read().syncbusy().bit_is_set() {}

    // Clear any pending interrupt
    eic.intflag.write(|w| w.extint4().set_bit());

    // Configure button pin for interrupt mode
    let button_pin = pins.pa04.into_pull_up_interrupt();

    // Set up deep sleep mode
    core.SCB.set_sleepdeep();

    // Enable the EIC interrupt in NVIC
    unsafe {
        NVIC::unmask(interrupt::EIC);
    }

    // Configure I2C pins (SDA on PA08, SCL on PA09)
    // For SERCOM0, both PA08 and PA09 need to be in Alternate C mode
    let sda = pins.pa08.into_alternate::<C>();
    let scl = pins.pa09.into_alternate::<C>();

    // Set up I2C for the DS3231 RTC
    let gclk0 = clocks.gclk0();
    let sercom0_core = clocks.sercom0_core(&gclk0).unwrap();
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

    // Main loop
    loop {
        blink_count += 1u32;
        led_pin.toggle().unwrap();

        // Example sensor readings
        let _current_time = rtc.get_time().unwrap();
        let _brightness: u16 = adc.read(&mut analog_pin).unwrap();

        delay.delay_ms(250u32);

        // Enter deep sleep after 8 blinks
        if blink_count >= 8 {
            // Turn LED off before sleep
            led_pin.set_low().unwrap();
            blink_count = 0;

            // Disable all peripherals to save power
            disable_peripherals(&peripherals.PM);

            // Enter sleep loop - wait for any interrupt (eg. button press)
            loop {
                // Enter standby mode (deepest sleep)
                cortex_m::asm::wfi();

                // Check if button is pressed
                if button_pin.is_low().unwrap() {
                    break;
                }
            }

            // Restore peripherals
            enable_peripherals(&peripherals.PM);
        }
    }
}

#[interrupt]
fn EIC() {
    // Clear the interrupt flag in the EIC
    unsafe {
        let eic = &(*EIC::ptr());
        // Clear interrupt for EXTINT4 (button on PA04)
        eic.intflag.modify(|_, w| w.extint4().set_bit());
    }
}

fn disable_peripherals(pm: &atsamd_hal::pac::PM) {
    // AHB bus - shut down almost everything
    pm.ahbmask.write(|w| {
        w.usb_().clear_bit()
         .dmac_().clear_bit()
         .nvmctrl_().set_bit()
         .dsu_().clear_bit()
    });

    // APBA bus - keep only EIC and SYSCTRL
    pm.apbamask.write(|w| {
        w.eic_().set_bit()
         .wdt_().clear_bit()
         .sysctrl_().set_bit()
         .pac0_().clear_bit()
         .pm_().set_bit()
    });

    // APBB bus - keep only PORT
    pm.apbbmask.write(|w| {
        w.usb_().clear_bit()
         .dmac_().clear_bit()
         .nvmctrl_().clear_bit()
         .dsu_().clear_bit()
         .port_().set_bit()
         .pac1_().clear_bit()
    });

    // APBC bus - disable everything
    pm.apbcmask.write(|w| {
        w.adc_().clear_bit()
         .sercom0_().clear_bit()
         .sercom1_().clear_bit()
         .sercom2_().clear_bit()
         .sercom3_().clear_bit()
         .tcc0_().clear_bit()
         .tcc1_().clear_bit()
         .tcc2_().clear_bit()
         .tc3_().clear_bit()
         .tc4_().clear_bit()
         .tc5_().clear_bit()
         .pac2_().clear_bit()
    });
}

fn enable_peripherals(pm: &atsamd_hal::pac::PM) {
    // AHB bus - restore all peripherals
    pm.ahbmask.write(|w| {
        w.usb_().set_bit()
         .dmac_().set_bit()
         .nvmctrl_().set_bit()
         .dsu_().set_bit()
    });

    // APBA bus - restore all peripherals
    pm.apbamask.write(|w| {
        w.eic_().set_bit()
         .wdt_().set_bit()
         .sysctrl_().set_bit()
         .pac0_().set_bit()
         .pm_().set_bit()
    });

    // APBB bus - restore all peripherals
    pm.apbbmask.write(|w| {
        w.usb_().set_bit()
         .dmac_().set_bit()
         .nvmctrl_().set_bit()
         .dsu_().set_bit()
         .port_().set_bit()
         .pac1_().set_bit()
    });

    // APBC bus - restore all peripherals
    pm.apbcmask.write(|w| {
        w.adc_().set_bit()
         .sercom0_().set_bit()
         .sercom1_().set_bit()
         .sercom2_().set_bit()
         .sercom3_().set_bit()
         .tcc0_().set_bit()
         .tcc1_().set_bit()
         .tcc2_().set_bit()
         .tc3_().set_bit()
         .tc4_().set_bit()
         .tc5_().set_bit()
         .pac2_().set_bit()
    });
}
