/***
 
 * SLAVE Device 
 */


#include <avr/io.h>

#define SLA_W     0x06 // (0x03<<1)
#define SLA_R     0x07 //(0x03<<1)|1

#define OWN_Address  0x03
#define GeneralCall   0

#define STARTCONDITON  (1<<TWSTA)|(1<<TWINT)|(1<<TWEN) // 0xA4
#define STOPCONDITON   (1<<TWSTO)|(1<<TWINT)|(1<<TWEN) // 
#define ENABLEACK      (1<<TWEA)|(1<<TWINT)|(1<<TWEN) // 
#define SEND           (1<<TWINT)|(1<<TWEN) // 

void init_TWI(unsigned char address);
void TWI_operate(unsigned char command);
unsigned char TWI_checkStatus();
void TWI_load(unsigned char load);
char TWI_SR(void);
void TWI_ST(char data);
unsigned char st;

int main(void) {
    DDRA = 0xFF;
    /* Replace with your application code */
    init_TWI(OWN_Address);
    TWI_ST(0x02);



    while (1) {
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

    // Waiting till Flag is raised so we can read the status value
    // and compare it with the corresponding table
    while (!(TWCR & (1 << TWINT)));

    return (TWSR & 0xF8);
}

void TWI_load(unsigned char load) {
    // Load either Address or Data 
    // Usually call SEND operation after it.
    TWDR = load;
    TWI_operate(SEND);
}

char TWI_SR(void){
    TWI_operate(ENABLEACK);
    st = TWI_checkStatus();
    if (st == 0x60) {
        TWI_operate(ENABLEACK);
        st = TWI_checkStatus();
        if (st == 0x80) {
            // Read Data
            return TWDR;
        }
        else{
            return 0;
        }
    }
    else{
        return 0;
    }
}

void TWI_ST(char data){
    TWI_operate(ENABLEACK);
    st = TWI_checkStatus();
    if (st == 0xA8) {
        TWDR = data;
        TWI_operate(SEND);
        st = TWI_checkStatus();
        if (st == 0xB8) {
            // Nothing to do unless there are more data to send.
        }
    }
}