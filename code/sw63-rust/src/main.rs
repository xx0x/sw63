#![no_std]
#![no_main]

mod ds3231;

use atsamd_hal::{
    adc::{Adc, Gain, Reference, Resolution, SampleRate},
    clock::GenericClockController,
    delay::Delay,
    gpio::v2::{Pins, B, C},
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
    let core = cortex_m::Peripherals::take().unwrap();

    // Configure clocks using the internal oscillator
    let mut clocks = GenericClockController::with_internal_32kosc(
        peripherals.GCLK,
        &mut peripherals.PM,
        &mut peripherals.SYSCTRL,
        &mut peripherals.NVMCTRL,
    );

    // Setup delay provider
    let mut delay = Delay::new(core.SYST, &mut clocks);

    // Configure pins
    let pins = Pins::new(peripherals.PORT);

    // Configure LED on PA23 and button on PA04
    let mut led_pin = pins.pa23.into_push_pull_output();
    let button_pin = pins.pa04.into_pull_up_input();

    // Configure I2C pins (SDA on PA08, SCL on PA09)
    // For SERCOM0, both PA08 and PA09 need to be in Alternate C mode
    let sda = pins.pa08.into_alternate::<C>();
    let scl = pins.pa09.into_alternate::<C>();

    // Setup I2C using SERCOM0
    let gclk0 = clocks.gclk0();
    let sercom0_core = clocks.sercom0_core(&gclk0).unwrap();

    // Set up I2C - create a master port with the given pins
    // using a simpler constructor directly with the pins
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
        blink_count += 1u32;
        led_pin.toggle().unwrap();

        // Read time
        let _current_time = rtc.get_time().unwrap();

        // Read brightness from PA02
        let _brightness: u16 = adc.read(&mut analog_pin).unwrap();

        // if button_pin.is_low().unwrap() {
        //     delay.delay_ms(50u32);
        // } else {
        delay.delay_ms(250u32);
        // }

        // Fake deep sleep
        if blink_count >= 8 {
            led_pin.set_low().unwrap();
            blink_count = 0;
            delay.delay_ms(100u32);
            while button_pin.is_high().unwrap() {
                // do nothing, just wait for button press
            }
        }
    }
}
