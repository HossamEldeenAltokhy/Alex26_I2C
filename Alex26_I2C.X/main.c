/***
 
 * MASTER  Device 
 */

#include <avr/io.h>

#define SLA_W     0x06 // (0x03<<1)
#define SLA_R     0x07 //(0x03<<1)|1

#define OWN_Address  0x07
#define GeneralCall   0

#define STARTCONDITON  (1<<TWSTA)|(1<<TWINT)|(1<<TWEN) // 0xA4
#define STOPCONDITON   (1<<TWSTO)|(1<<TWINT)|(1<<TWEN) // 
#define ENABLEACK      (1<<TWEA)|(1<<TWINT)|(1<<TWEN) // 
#define SEND           (1<<TWINT)|(1<<TWEN) // 

struct TWI_Config {
    unsigned char address;
    char data;
};
struct TWI_Config mTWI_Config = {SLA_W, 9};





void init_TWI(unsigned char address);
void TWI_operate(unsigned char command);
unsigned char TWI_checkStatus();
void TWI_load(unsigned char load);
void TWI_MT(unsigned char address, char data);
char TWI_MR(unsigned char address);
void mTWI_MT(struct TWI_Config* pTWI_Config);
unsigned char st;

int main(void) {
    DDRA = 0xFF;
    /* Replace with your application code */
    init_TWI(OWN_Address);
    PORTA = TWI_MR(SLA_R);



    while (1) {
    }
}

void TWI_MT(unsigned char address, char data) {
    TWI_operate(STARTCONDITON);
    st = TWI_checkStatus();
    if (st == 0x08) {
        // Load SLA+W
        TWI_load(address);
        TWI_operate(SEND);
        st = TWI_checkStatus();
        if (st == 0x18) { ///******///
            TWI_load(data);
            TWI_operate(SEND);
            st = TWI_checkStatus();
            if (st == 0x28) {
                TWI_operate(STOPCONDITON);
            }
        }
    }
}

char TWI_MR(unsigned char address) {
    TWI_operate(STARTCONDITON);
    st = TWI_checkStatus();
    if (st == 0x08) {
        // Load SLA+W
        TWI_load(address);
        TWI_operate(SEND);
        st = TWI_checkStatus();
        if (st == 0x40) { ///******///
            TWI_operate(ENABLEACK);
            st = TWI_checkStatus();
            if (st == 0x50) {
                char data = TWDR;
                TWI_operate(STOPCONDITON);
                return data;
            } else {
                return 0;
            }
        } else {
            return 0;
        }
    } else {
        return 0;
    }
}

void mTWI_MT(struct TWI_Config* pTWI_Config) {

    TWI_operate(STARTCONDITON);
    st = TWI_checkStatus();
    if (st == 0x08) {
        // Load SLA+W
        TWI_load(pTWI_Config->address);
        TWI_operate(SEND);
        st = TWI_checkStatus();
        if (st == 0x18) { ///******///
            TWI_load(pTWI_Config->data);
            TWI_operate(SEND);
            st = TWI_checkStatus();
            if (st == 0x28) {
                TWI_operate(STOPCONDITON);
            }
        }
    }
}

void init_TWI(unsigned char address) {

    // Set Clock Freq
    // Set Prescalar >> TWPS >> TWSR
    TWSR &= ~((1 << TWPS1) | (1 << TWPS0));
    // Set TWBR value
    TWBR = 100;
    // Set my OWN address
    TWAR = (address << 1) | GeneralCall; // 0x01; // 00000001

}

void TWI_operate(unsigned char command) {
    TWCR = command;
}

unsigned char TWI_checkStatus() {

    while (!(TWCR & (1 << TWINT)));

    return (TWSR & 0xF8);
}

void TWI_load(unsigned char load) {
    TWDR = load;
    TWI_operate(SEND);
}