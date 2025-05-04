#![no_std]
#![no_main]

use panic_halt as _;
use cortex_m_rt::entry;
use atsamd_hal::{
    clock::GenericClockController,
    delay::Delay,
    prelude::*,
    target_device::Peripherals,
    gpio::v2::{Pin, PA23, Output, PushPull},
};

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
    let pins = atsamd_hal::gpio::v2::Pins::new(peripherals.PORT);
    let mut led_pin: Pin<PA23, Output<PushPull>> = pins.pa23.into_push_pull_output();
    
    // Main loop - blink LED
    loop {
        led_pin.toggle().unwrap();
        delay.delay_ms(50u32);
    }
}
