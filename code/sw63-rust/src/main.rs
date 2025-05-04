#![no_std]
#![no_main]

use atsamd_hal::{
    clock::GenericClockController,
    delay::Delay,
    gpio::v2::{Input, Output, Pin, Pins, PullUp, PushPull, PA04, PA23},
    prelude::*,
    target_device::Peripherals,
};
use cortex_m_rt::entry;
use panic_halt as _;

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

    // Configure pins - PA23 as output pin for the LED
    let pins = Pins::new(peripherals.PORT);

    let mut led_pin: Pin<PA23, Output<PushPull>> = pins.pa23.into_push_pull_output();
    let button_pin: Pin<PA04, Input<PullUp>> = pins.pa04.into_pull_up_input();

    // Main loop - blink LED
    loop {
        led_pin.toggle().unwrap();
        let mut delay_time = 500u32;
        if button_pin.is_low().unwrap() {
            delay_time = 50u32;
        }
        delay.delay_ms(delay_time);
    }
}
