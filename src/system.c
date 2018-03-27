/*******************************************************************************
  System Initialization for Microstick

  Company:
    Microchip Technology Inc.

  File Name:
    system.c

  Summary:
    This file contains configuration bit definitions for use with the Microstick
    Demo.

  Description:
    This file contains configuration bit definitions for use with the Microstick
    Demo.
*******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2013 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
*******************************************************************************/
// DOM-IGNORE-END


// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
/*  This section lists the other files that are included in this file.
 */

// *****************************************************************************
// *****************************************************************************
// Section: Configuration Bits
// *****************************************************************************
// *****************************************************************************

//FBS
// BWRP Boot Segment Write Protect
#pragma config BWRP            = OFF                  //Disabled                                
// BSS Boot segment Protect
#pragma config BSS             = OFF                  //No boot program flash segment           

//FGS
// GWRP General Segment Write Protect
#pragma config GWRP            = OFF                  //General segment may be written          
// GCP General Segment Code Protect
#pragma config GCP             = OFF                  //No Protection                           

//FOSCSEL
// FNOSC Oscillator Select
#pragma config FNOSC           = FRCPLL               //Fast RC Oscillator with Postscaler and PLL Module (FRCDIV+PLL)
// SOSCSRC SOSC Source Type
#pragma config SOSCSRC         = DIG                  //Digital Mode for use with external source
// LPRCSEL LPRC Oscillator Power and Accuracy
#pragma config LPRCSEL         = HP                   //High Power, High Accuracy Mode          
// IESO Internal External Switch Over bit
#pragma config IESO            = OFF                  //Internal External Switchover mode disabled (Two-speed Start-up disabled)

//FOSC
// POSCMOD Primary Oscillator Configuration bits
#pragma config POSCMOD         = NONE                 //Primary oscillator disabled             
// OSCIOFNC CLKO Enable Configuration bit
#pragma config OSCIOFNC        = IO                   //Port I/O enabled (CLKO disabled)        
// SOSCSEL SOSC Power Selection Configuration bits
#pragma config SOSCSEL         = SOSCHP               //Secondary Oscillator configured for high-power operation
// FCKSM Clock Switching and Monitor Selection
#pragma config FCKSM           = CSDCMD               //Both Clock Switching and Fail-safe Clock Monitor are disabled

//FWDT
// WDTPS Watchdog Timer Postscale Select bits
#pragma config WDTPS           = PS32768              //1:32768                                 
// FWPSA WDT Prescaler bit
#pragma config FWPSA           = PR128                //WDT prescaler ratio of 1:128            
// FWDTEN Watchdog Timer Enable bits
#pragma config FWDTEN          = NOSLP                //WDT enabled while device is active and disabled in Sleep; SWDTEN bit disabled
// WINDIS Windowed Watchdog Timer Disable bit
#pragma config WINDIS          = OFF                  //Standard WDT selected(windowed WDT disabled)

//FPOR
// BOREN Brown-out Reset Enable bits
#pragma config BOREN           = BOR2                 //Brown-out Reset enabled only while device is active and disabled in SLEEP, SBOREN bit disabled
// RETCFG 
#pragma config RETCFG          = ON                   //Retention regulator is available and controlled by RETEN bit
// PWRTEN Power-up Timer Enable bit
#pragma config PWRTEN          = OFF                  //PWRT disabled                           


/*******************************************************************************
 End of File
*/
