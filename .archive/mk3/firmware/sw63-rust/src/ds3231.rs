//! DS3231 Real-Time Clock driver
//! 
//! This module provides a driver for the DS3231 Real-Time Clock with
//! calendar functionality.

use atsamd_hal::hal::blocking::i2c::{Write, WriteRead};

/// DS3231 device address
pub const DS3231_ADDR: u8 = 0x68;

/// Register address for time/date data
pub const REG_TIME: u8 = 0x00;

/// Base year for the DS3231 (years are stored as 0-99)
pub const BASE_YEAR: u16 = 2000;

/// Time struct to hold date and time information
#[derive(Debug, Clone, Copy)]
pub struct Time {
    pub second: u8,
    pub minute: u8,
    pub hour: u8,
    pub day: u8,
    pub month: u8,
    pub year: u16,
}

/// DS3231 driver
pub struct DS3231<I2C> {
    i2c: I2C,
}

impl<I2C, E> DS3231<I2C>
where
    I2C: Write<Error = E> + WriteRead<Error = E>,
{
    /// Create a new DS3231 driver with the given I2C interface
    pub fn new(i2c: I2C) -> Self {
        Self { i2c }
    }

    /// Set the time and date on the DS3231
    pub fn set_time(&mut self, time: &Time) -> Result<(), E> {
        // Ensure year is between 0 and 99 (relative to BASE_YEAR)
        let year_fixed = if time.year >= BASE_YEAR {
            (time.year - BASE_YEAR) as u8
        } else {
            0
        };
        
        // Convert to BCD format
        let data = [
            REG_TIME, // Register address (time registers start at 0)
            dec_to_bcd(time.second),
            dec_to_bcd(time.minute),
            dec_to_bcd(time.hour),
            0, // Day of week (not used)
            dec_to_bcd(time.day),
            dec_to_bcd(time.month),
            dec_to_bcd(year_fixed),
        ];

        // Write time and date to the DS3231
        self.i2c.write(DS3231_ADDR, &data)
    }

    /// Read the current time from the DS3231
    pub fn get_time(&mut self) -> Result<Time, E> {
        let reg_addr = [REG_TIME]; // Register address (time registers start at 0)
        let mut data = [0u8; 7];
        
        // Read 7 bytes of time/date data starting from register 0
        self.i2c.write_read(DS3231_ADDR, &reg_addr, &mut data)?;
        
        // Convert BCD to decimal and return as Time struct
        Ok(Time {
            second: bcd_to_dec(data[0]),
            minute: bcd_to_dec(data[1]),
            hour: bcd_to_dec(data[2] & 0x3F), // Mask AM/PM bit for 24-hour format
            day: bcd_to_dec(data[4]),
            month: bcd_to_dec(data[5] & 0x1F), // Mask century bit
            year: BASE_YEAR + bcd_to_dec(data[6]) as u16,
        })
    }
    
    // Release the I2C bus
    // pub fn release(self) -> I2C {
    //     self.i2c
    // }
}

/// Convert BCD format to decimal
fn bcd_to_dec(bcd: u8) -> u8 {
    (bcd & 0x0F) + ((bcd >> 4) * 10)
}

/// Convert decimal to BCD format
fn dec_to_bcd(dec: u8) -> u8 {
    ((dec / 10) << 4) | (dec % 10)
}