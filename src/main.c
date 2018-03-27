// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
/*  This section lists the other files that are included in this file.
 */
#define FCY  (16000000UL)
#include <stdint.h>
#include <libpic30.h> 

#include <xc.h>
#include <i2c.h>
#include <spi.h>

#define LED                LATAbits.LATA0
#define LED_TRIS           TRISAbits.TRISA0
#define SDA                PORTBbits.RB9
#define SCL                PORTBbits.RB8
#define SDA_TRIS           TRISBbits.TRISB9
#define SCL_TRIS           TRISBbits.TRISB8
#define MODE_SWITCH        PORTBbits.RB7
#define MODE_TRIS          TRISBbits.TRISB7

unsigned char eq_off[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

// TDA7348 Commands
unsigned char mute[] = {0x08, 0x81};
unsigned char mute_spk[] = {0x14, 0x1F, 0x1F, 0x1F, 0x1F};
unsigned char bass_treb[] = {0x03, 0xDD};
unsigned char input_aux = 0x32;
unsigned char input_CD = 0x39;
unsigned char mute_spk_off[] = {0x14, 0x00, 0x00, 0x00, 0x00};
unsigned char mute_off[] = {0x08, 0x80};

unsigned char switchInput(unsigned char input);
unsigned char sendToTDA7348(unsigned char* data, unsigned char length);
unsigned char sendToEQ(unsigned char* data, unsigned char length);

// I2C Sniffer
unsigned char SDANew, SDAOld;
unsigned char SCLNew, SCLOld;

unsigned char DataState = 0;
unsigned char DataBits = 0;
unsigned char DataBytes = 0;
unsigned char dat = 0;
unsigned char sniffing = 0;
unsigned char data[12];

// SPI Button reception
unsigned char spi_values[4] = {0};
unsigned char curr_in = 0;
unsigned char in_count = 0;
// CD Button value
unsigned char button_val[4] = {0x01, 0x0A, 0x1B, 0x26};

// State of Radio
#define RADIO 0
#define CD 1
#define AUX 3
unsigned char selectedInput = CD;

unsigned char changeToAux = 0;
unsigned char changeToCD = 0;

unsigned char volume1 = 0x10;
unsigned char volume2 = 0xCC;

void start_I2C_Sniffer();
void service_I2C_Sniffer();
void configure_SPI2();
void spi_read();

int main(void) {
    // Port B pins to digital
    ANSB = 0;
    // Start out LED off
    LED = 0;
    LED_TRIS = 0;
    MODE_TRIS = 1;
    SDA_TRIS = 1;
    SCL_TRIS = 1;
    unsigned char attempts = 0;
    unsigned char switchRes = 0;
    unsigned char switching = 0;
    start_I2C_Sniffer();
    configure_SPI2();
    RCONbits.SWDTEN = 1;
    for (;;) {
        ClrWdt();
        if (DataRdySPI2()) {
            // We need to read the data.
            spi_read();
        }

        if (MODE_SWITCH == 1 && switching) {
            switching = 0;
        } else if (MODE_SWITCH == 0) {
            if (switching == 0) {
                __delay_ms(10);
                if (MODE_SWITCH == 1) {
                    continue;
                }
                switching = 1;
                if (selectedInput == CD) {
                    changeToAux = 1;
                } else {
                    changeToCD = 1;
                }
            }
        }
        if (changeToAux) {
            changeToAux = 0;
            switchRes = switchInput(input_aux);
            while (switchRes && attempts < 10) {
                __delay_ms(10);
                ++attempts;
                switchRes = switchInput(input_aux);
            }
            if (switchRes == 0) {
                LED = 1;
                selectedInput = AUX;
            } else {
                LED = 1;
                __delay_ms(50);
                LED = 0;
                __delay_ms(50);
                LED = 1;
                __delay_ms(50);
                LED = 0;
                __delay_ms(50);
                LED = 1;
                __delay_ms(50);
                LED = 0;
            }
            attempts = 0;
        } else if (changeToCD) {
            changeToCD = 0;
            switchRes = switchInput(input_CD);
            while (switchRes && attempts < 10) {
                __delay_ms(10);
                ++attempts;
                switchRes = switchInput(input_CD);
            }
            if (switchRes == 0) {
                LED = 0;
                selectedInput = CD;
            }
            attempts = 0;
        }
        service_I2C_Sniffer();

    }
    return 0;
}

void configure_SPI2() {
    // Setup for slave operation.
    SSP2STATbits.SMP = 0;
    SSP2CON1bits.SSPM = 0b0100;
    SSP2CON1bits.CKP = 1;
    SSP2CON1bits.SSPEN = 1;
}

void spi_read() {

    curr_in = SSP2BUF;
    if (curr_in == 0x00) {
        in_count = 0;
    } else {
        if (in_count < 4) {
            spi_values[in_count] = curr_in;
            if (++in_count == 4) {
                in_count = 0;
                int i = 0;
                for (i = 0; i < 4; ++i) {
                    if (spi_values[i] != button_val[i]) {
                        return;
                    }
                }
                if (selectedInput == CD) {
                    // Trigger change to AUX mode
                    changeToAux = 1;
                } else if (selectedInput == AUX) {
                    changeToCD = 1;
                }
            }
        }
    }
}

void OpenI2C1_Master() {
    SSP1CON1bits.SSPEN = 1;
    SSP1CON1bits.SSPM = 0b1000;
    SSP1CON1bits.CKP = 1;
    SSP1ADD = 0x9F;
}

/*
 * Sends data to the TDA7348 I2C controlled audio processor.
 *
 * Returns 0 on success, 2 on collision and 1 on NACK
 */
unsigned char sendToTDA7348(unsigned char* data, unsigned char length) {
    unsigned char i = 0;
    IdleI2C1();
    signed char writeRes = 0;
    writeRes = WriteI2C1(0x88);
    if (writeRes == -1) {
        return 2;
    } else if (writeRes == -2) {
        return 1;
    }
    for (i = 0; i < length; ++i) {
        writeRes = WriteI2C1(data[i]);
        if (writeRes == -1) {
            return 2;
        } else if (writeRes == -2) {
            return 1;
        }
    }
    return 0;
}

unsigned char sendAuxCommand() {
    char writeRes = 0;
    IdleI2C1(); // test for idle condition
    StartI2C1();
    IdleI2C1();
    writeRes = WriteI2C1(0x88);
    if (writeRes == -1) {
        return 2;
    } else if (writeRes == -2) {
        return 1;
    }
    writeRes = WriteI2C1(0x00);
    if (writeRes == -1) {
        return 2;
    } else if (writeRes == -2) {
        return 1;
    }
    writeRes = WriteI2C1(input_aux);
    if (writeRes == -1) {
        return 2;
    } else if (writeRes == -2) {
        return 1;
    }
    StopI2C1();
    return 0;
}

unsigned char sendVolCommand(unsigned char command) {
    signed char writeRes = 0;
    IdleI2C1();
    writeRes = WriteI2C1(0x88);
    if (writeRes == -1) {
        return 2;
    } else if (writeRes == -2) {
        return 1;
    }
    writeRes = WriteI2C1(0x02);
    if (writeRes == -1) {
        return 2;
    } else if (writeRes == -2) {
        return 1;
    }
    writeRes = WriteI2C1(command);
    if (writeRes == -1) {
        return 2;
    } else if (writeRes == -2) {
        return 1;
    }
    StopI2C1();
    IdleI2C1();
    StartI2C1();
    IdleI2C1();
    writeRes = WriteI2C1(0x88);
    if (writeRes == -1) {
        return 2;
    } else if (writeRes == -2) {
        return 1;
    }
    writeRes = WriteI2C1(0x01);
    if (writeRes == -1) {
        return 2;
    } else if (writeRes == -2) {
        return 1;
    }
    writeRes = WriteI2C1(volume1);
    if (writeRes == -1) {
        return 2;
    } else if (writeRes == -2) {
        return 1;
    }
    return 0;
}

unsigned char sendToEQ(unsigned char* data, unsigned char length) {
    unsigned char i = 0;
    signed char writeRes = 0;
    IdleI2C1();
    writeRes = WriteI2C1(0x84);
    if (writeRes == -1) {
        return 2;
    } else if (writeRes == -2) {
        return 1;
    }
    for (i = 0; i < length; ++i) {
        writeRes = WriteI2C1(data[i]);
        if (writeRes == -1) {
            return 2;
        } else if (writeRes == -2) {
            return 1;
        }
    }
    return 0;
}

unsigned char switchInput(unsigned char input) {
    unsigned char res;
    unsigned char currVol = 0xF8;
    OpenI2C1_Master();
    IdleI2C1();
    StartI2C1();
    res = sendToTDA7348(mute, 2);
    if (res == 1) {
        goto StopSend;
    } else if (res == 2) {
        goto Close;
    }
    StopI2C1();
    IdleI2C1(); // test for idle condition
    StartI2C1();
    res = sendToTDA7348(mute_spk, 5);
    if (res == 1) {
        goto StopSend;
    } else if (res == 2) {
        goto Close;
    }
    StopI2C1();
    IdleI2C1(); // test for idle condition
    StartI2C1();
    res = sendToEQ(eq_off, 6);
    if (res == 1) {
        goto StopSend;
    } else if (res == 2) {
        goto Close;
    }
    StopI2C1();
    IdleI2C1(); // test for idle condition
    StartI2C1();
    res = sendVolCommand(currVol);
    if (res == 1) {
        goto StopSend;
    } else if (res == 2) {
        goto Close;
    }
    StopI2C1();
    IdleI2C1(); // test for idle condition
    StartI2C1();
    IdleI2C1();
    signed char writeRes = 0;
    writeRes = WriteI2C1(0x88);
    if (writeRes == -1) {
        goto Close;
    } else if (writeRes == -2) {
        goto StopSend;
    }
    writeRes = WriteI2C1(0x00);
    if (writeRes == -1) {
        goto Close;
    } else if (writeRes == -2) {
        goto StopSend;
    }
    writeRes = WriteI2C1(input);
    if (writeRes == -1) {
        goto Close;
    } else if (writeRes == -2) {
        goto StopSend;
    }
    StopI2C1();
    IdleI2C1(); // test for idle condition
    StartI2C1();
    res = sendToTDA7348(mute_spk_off, 5);
    if (res == 1) {
        goto StopSend;
    } else if (res == 2) {
        goto Close;
    }
    StopI2C1();
    IdleI2C1(); // test for idle condition
    StartI2C1();
    res = sendToTDA7348(mute_off, 2);
    if (res == 1) {
        goto StopSend;
    } else if (res == 2) {
        goto Close;
    }
    StopI2C1();
    while (currVol > volume2) {
        __delay_ms(25);
        currVol -= 4;
        sendVolCommand(currVol);
    }
    sendVolCommand(currVol);
    CloseI2C1();
    return 0;

StopSend:
    StopI2C1();
Close:
    CloseI2C1();
    return 1;
}

void start_I2C_Sniffer() {
    DataState = 0;
    DataBits = 0;
    DataBytes = 0;
    dat = 0;
    SDA_TRIS = 1; // -- Ensure pins are in high impedance mode --
    SCL_TRIS = 1;
    sniffing = 1;
    SCL = 0; // writes to the PORTs write to the LATCH
    SDA = 0;
    SDAOld = SDANew = SDA;
    SCLOld = SDANew = SCL;
    service_I2C_Sniffer();
}

void service_I2C_Sniffer() {

    if (!sniffing) {
        start_I2C_Sniffer();
    }
    SDANew = SDA; //store current state right away
    SCLNew = SCL;

    if (DataState && !SCLOld && SCLNew) // Sample When SCL Goes Low To High
    {
        if (DataBits < 8) //we're still collecting data bits
        {
            dat = dat << 1;
            if (SDANew) {
                dat |= 1;
            }

            DataBits++;
        } else {
            if (DataBytes < 12) {
                data[DataBytes] = dat;
                ++DataBytes;
            }
            DataBits = 0; // Ready For Next Data Byte
        }
    } else if (SCLOld && SCLNew) // SCL High, Must Be Data Transition
    {
        if (SDAOld && !SDANew) // Start Condition (High To Low)
        {
            DataState = 1; // Allow Data Collection
            DataBits = 0;
            DataBytes = 0;

        } else if (!SDAOld && SDANew) // Stop Condition (Low To High)
        {
            DataState = 0; // Don't Allow Data Collection
            DataBits = 0;
            if (DataBytes == 3) {
                // Prevent unit from switching back to CD mode

                // Master talking to TDA7348
                if (data[0] == 0x88) {
                    // Changing input
                    if (data[1] == 0x00) {
                        if (data[2] == 0x35) {
                            selectedInput = RADIO;
                            LED = 0;
                        } else if (data[2] == input_CD) {
                            if (selectedInput == AUX) {
                                unsigned char res;
                                __delay_ms(3);
                                OpenI2C1_Master();
                                res = sendAuxCommand();
                                if (res) {
                                    __delay_ms(3);
                                    sendAuxCommand();
                                }
                                CloseI2C1();
                                return;
                            } else {
                                selectedInput = CD;
                            }
                        }
                    }
                    // Volume values
                    if (data[1] == 0x01) {
                        volume1 = data[2];
                    }
                    if (data[1] == 0x02) {

                        volume2 = data[2];
                    }
                }
            }
        }
    }

    SDAOld = SDANew; // Save Last States
    SCLOld = SCLNew;
}
