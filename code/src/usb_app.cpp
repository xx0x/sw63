#include "tusb.h"
#include "dev/Communication.hpp"

extern "C" {

//--------------------------------------------------------------------
// Device callbacks
//--------------------------------------------------------------------

// Invoked when device is mounted
void tud_mount_cb(void)
{
    // Device is now configured and can communicate
}

// Invoked when device is unmounted
void tud_umount_cb(void)
{
    // Device is disconnected
}

// Invoked when usb bus is suspended
// remote_wakeup_en : if host allow us to perform remote wakeup
void tud_suspend_cb(bool remote_wakeup_en)
{
    (void) remote_wakeup_en;
}

// Invoked when usb bus is resumed
void tud_resume_cb(void)
{
    
}

//--------------------------------------------------------------------
// CDC callbacks
//--------------------------------------------------------------------

// Invoked when cdc when line state changed e.g connected/disconnected
void tud_cdc_line_state_cb(uint8_t itf, bool dtr, bool rts)
{
    (void) itf;
    (void) rts;

    // TODO: Use DTR for line state detection if needed
    if ( dtr )
    {
        // Terminal connected
    }
    else
    {
        // Terminal disconnected
    }
}

// Invoked when CDC interface received data from host
void tud_cdc_rx_cb(uint8_t itf)
{
    (void) itf;
    // Data will be processed in Communication::Process()
}

// Invoked when space becomes available in TX buffer
void tud_cdc_tx_complete_cb(uint8_t itf)
{
    (void) itf;
}

// Invoked when received `wanted_char`
void tud_cdc_rx_wanted_cb(uint8_t itf, char wanted_char)
{
    (void) itf;
    (void) wanted_char;
}

// Invoked when received new data
void tud_cdc_line_coding_cb(uint8_t itf, cdc_line_coding_t const* p_line_coding)
{
    (void) itf;
    (void) p_line_coding;
}

} // extern "C"