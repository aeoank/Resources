#include <stdio.h>

#define  Finger_RST_Pin     14
#define  Finger_WAKE_Pin    33

#define TRUE  1
#define FALSE 0

// Basic response message definition
#define ACK_SUCCESS           0x00
#define ACK_FAIL              0x01
#define ACK_FULL	            0x04
#define ACK_NO_USER	          0x05
#define ACK_USER_OCCUPIED     0x06
#define ACK_FINGER_OCCUPIED   0x07
#define ACK_TIMEOUT           0x08

//User information definition
#define ACK_ALL_USER       	  0x00
#define ACK_GUEST_USER 	  	  0x01
#define ACK_NORMAL_USER 	    0x02
#define ACK_MASTER_USER    	  0x03

#define USER_MAX_CNT	   	500	// Maximum fingerprint number

// Command definition
#define CMD_HEAD	            0xF5
#define CMD_TAIL	            0xF5

#define CMD_ADD_1             0x01
#define CMD_ADD_2             0x02
#define CMD_ADD_3	            0x03
#define CMD_MATCH	            0x0C
#define CMD_DEL		            0x04
#define CMD_DEL_ALL	          0x05
#define CMD_USER_CNT          0x09
#define CMD_COM_LEV	          0x28
#define CMD_LP_MODE           0x2C
#define CMD_TIMEOUT           0x2E
#define CMD_FINGER_DETECTED   0x14


extern  uint8_t  Finger_SleepFlag;


void Finger_HardwareSerial_Init(void);
void  TxByte(uint8_t temp);
uint8_t GetUserCount(void);
uint8_t GetcompareLevel(void);
uint8_t SetcompareLevel(uint8_t temp);	// The default value is 5, can be set to 0-9, the bigger, the stricter
uint8_t AddUser(void);
uint8_t ClearAllUser(void);
uint8_t VerifyUser(void);
uint8_t GetTimeOut(void);
void Finger_Wait_Until_OK(void);	
void Analysis_PC_Command(void);
void Auto_Verify_Finger(void);

uint8_t finger_RxBuf[9];			
uint8_t finger_TxBuf[9];	

uint8_t  Finger_SleepFlag = 0;
int user_id;





void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial1.begin(19200, SERIAL_8N1, 16, 17);
	while(!Serial); 
 pinMode(2, OUTPUT);
 digitalWrite(2, LOW);
 //Configure the RST and Wakeup pins
 pinMode(Finger_RST_Pin, OUTPUT);
 digitalWrite(Finger_RST_Pin, HIGH);
 
 pinMode(Finger_WAKE_Pin, INPUT);

//  Finger_HardwareSerial_Init(); 
//  delay( 1000);
 
 Finger_Wait_Until_OK();
}

void loop() {
  // put your main code here, to run repeatedly:
	  Analysis_PC_Command();
  if(Finger_SleepFlag == 1)
  {
    Auto_Verify_Finger();
  } 
}

void  TxByte(uint8_t temp){
	Serial1.write(temp);    
}

uint8_t TxAndRxCmd(uint8_t Scnt, uint8_t Rcnt, uint16_t Nms){
	uint8_t  i, j, CheckSum;
	uint16_t   uart_RxCount = 0;
	unsigned long  time_before = 0;
	unsigned long  time_after = 0;
	uint8_t   overflow_Flag = 0;;	
	
	 TxByte(CMD_HEAD);		 
	 CheckSum = 0;
	 for (i = 0; i < Scnt; i++)
	 {
		TxByte(finger_TxBuf[i]);		 
		CheckSum ^= finger_TxBuf[i];
	 }	
	 TxByte(CheckSum);
	 TxByte(CMD_TAIL);  
	 
	 memset(finger_RxBuf,0,sizeof(finger_RxBuf));   ////////
 
	 Serial1.flush();  /////
	 
	 // Receive time out: Nms
	time_before = millis();	 
	 do
	 {
		overflow_Flag = 0;
		if(Serial1.available())
		{
			finger_RxBuf[uart_RxCount++] = Serial1.read();
		}
		time_after = millis();	
		if(time_before > time_after)   //if overflow (go back to zero)
		{
			time_before = millis();	  // get time_before again
			overflow_Flag = 1;
		}
		
	 } while (((uart_RxCount < Rcnt) && (time_after - time_before < Nms)) || (overflow_Flag == 1));

   user_id=finger_RxBuf[2]*0 +finger_RxBuf[3];
	 if (uart_RxCount != Rcnt)return ACK_TIMEOUT;
	 if (finger_RxBuf[0] != CMD_HEAD) return ACK_FAIL;
	 if (finger_RxBuf[Rcnt - 1] != CMD_TAIL) return ACK_FAIL;
	 if (finger_RxBuf[1] != (finger_TxBuf[0])) return ACK_FAIL;	 
	 CheckSum = 0;
	 for (j = 1; j < uart_RxCount - 1; j++) CheckSum ^= finger_RxBuf[j];
	 if (CheckSum != 0) return ACK_FAIL; 	  
 	 return ACK_SUCCESS;
}	 

uint8_t GetUserCount(void){
  uint8_t m;
	
	finger_TxBuf[0] = CMD_USER_CNT;
	finger_TxBuf[1] = 0;
	finger_TxBuf[2] = 0;
	finger_TxBuf[3] = 0;
	finger_TxBuf[4] = 0;	
	
	m = TxAndRxCmd(5, 8, 200);
			
	if (m == ACK_SUCCESS && finger_RxBuf[4] == ACK_SUCCESS)
	{
	    return finger_RxBuf[3];
	}
	else
	{
	 	return 0xFF;
	}
}

uint8_t GetcompareLevel(void){
	uint8_t m;
	
	finger_TxBuf[0] = CMD_COM_LEV;
	finger_TxBuf[1] = 0;
	finger_TxBuf[2] = 0;
	finger_TxBuf[3] = 1;
	finger_TxBuf[4] = 0;	
	
	m = TxAndRxCmd(5, 8, 200);
		
	if (m == ACK_SUCCESS && finger_RxBuf[4] == ACK_SUCCESS)
	{
	    return finger_RxBuf[3];
	}
	else
	{
	 	return 0xFF;
	}
}
uint8_t SetcompareLevel(uint8_t temp){
	uint8_t m;
	
	finger_TxBuf[0] = CMD_COM_LEV;
	finger_TxBuf[1] = 0;
	finger_TxBuf[2] = temp;
	finger_TxBuf[3] = 0;
	finger_TxBuf[4] = 0;	
	
	m = TxAndRxCmd(5, 8, 200);

	if (m == ACK_SUCCESS && finger_RxBuf[4] == ACK_SUCCESS)
	{
	    return finger_RxBuf[3];
	}
	else
	{
	 	return 0xFF;
	}
}

uint8_t GetTimeOut(void){
	uint8_t m;
	
	finger_TxBuf[0] = CMD_TIMEOUT;
	finger_TxBuf[1] = 0;
	finger_TxBuf[2] = 0;
	finger_TxBuf[3] = 1;
	finger_TxBuf[4] = 0;	
	
	m = TxAndRxCmd(5, 8, 200);
		
	if (m == ACK_SUCCESS && finger_RxBuf[4] == ACK_SUCCESS)
	{
	    return finger_RxBuf[3];
	}
	else
	{
	 	return 0xFF;
	}
}

uint8_t AddUser(void){
	uint8_t m;
	
	m = GetUserCount();
	if (m >= USER_MAX_CNT)
		return ACK_FULL;


	finger_TxBuf[0] = CMD_ADD_1;
	finger_TxBuf[1] = 0;
	finger_TxBuf[2] = m +1;
	finger_TxBuf[3] = 3;
	finger_TxBuf[4] = 0;		
	m = TxAndRxCmd(5, 8, 6000);	
	if (m == ACK_SUCCESS && finger_RxBuf[4] == ACK_SUCCESS)
	{
		finger_TxBuf[0] = CMD_ADD_2;
		m = TxAndRxCmd(5, 8, 6000);
    if (m == ACK_SUCCESS && finger_RxBuf[4] == ACK_SUCCESS)
  {
    finger_TxBuf[0] = CMD_ADD_3;
    m = TxAndRxCmd(5, 8, 6000);
		if (m == ACK_SUCCESS && finger_RxBuf[4] == ACK_SUCCESS)
		{
			return ACK_SUCCESS;
		}
		else
		return ACK_FAIL;
	}
  else
    return ACK_FAIL;
	}
	else
		return ACK_FAIL;
}

uint8_t  ClearAllUser(void)
{
 	uint8_t m;
	
	finger_TxBuf[0] = CMD_DEL_ALL;
	finger_TxBuf[1] = 0;
	finger_TxBuf[2] = 0;
	finger_TxBuf[3] = 0;
	finger_TxBuf[4] = 0;
	
	m = TxAndRxCmd(5, 8, 500);
	
	if (m == ACK_SUCCESS && finger_RxBuf[4] == ACK_SUCCESS)
	{	    
		return ACK_SUCCESS;
	}
	else
	{
		return ACK_FAIL;
	}
}

uint8_t IsMasterUser(uint8_t UserID){
    if ((UserID == 1) || (UserID == 2) || (UserID == 3)) return TRUE;
			else  return FALSE;
}	 

uint8_t VerifyUser(void)
{
	uint8_t m;
	
	finger_TxBuf[0] = CMD_MATCH;
	finger_TxBuf[1] = 0;
	finger_TxBuf[2] = 0;
	finger_TxBuf[3] = 0;
	finger_TxBuf[4] = 0;
	
	m = TxAndRxCmd(5, 8, 5000);
		
	if ((m == ACK_SUCCESS) && (IsMasterUser(finger_RxBuf[4]) == TRUE) && finger_RxBuf[3] != 0)
	{	
		 return ACK_SUCCESS;
	}
	else if(finger_RxBuf[4] == ACK_NO_USER)
	{
		return ACK_NO_USER;
	}
	else if(finger_RxBuf[4] == ACK_TIMEOUT)
	{
		return ACK_TIMEOUT;
	}
  else{
    return ACK_FAIL;
  }

}

void Finger_Wait_Until_OK(void)
{		
    digitalWrite(Finger_RST_Pin , LOW);
	delay(300); 
    digitalWrite(Finger_RST_Pin , HIGH);
	delay(300);  // Wait for module to start
    
	 // ERROR: Please ensure that the module power supply is 3.3V or 5V, 
	 // the serial line is correct, the baud rate defaults to 19200,
	 // and finally the power is switched off, and then power on again !
	while(SetcompareLevel(5) != 5)
	{		
		Serial.println("*Please waiting for communicating normally, if it always keep waiting here, please check your connection!*");
	}

	Serial.write("*************** WaveShare Capacitive Fingerprint Reader Test ***************\r\n");
	Serial.write("Compare Level:  5    (can be set to 0-9, the bigger, the stricter)\r\n"); 
	Serial.write("Number of fingerprints already available:  ");  Serial.print(GetUserCount());
	Serial.write("\r\n Use the serial port to send the commands to operate the module:\r\n"); 
	Serial.write(" CMD1 : Query the number of existing fingerprints\r\n"); 
	Serial.write(" CMD2 : Add fingerprint  (Each entry needs to be read two times: \"beep\", "); Serial.write("put the finger on sensor\r\n"); 
	Serial.write(" CMD3 : Fingerprint matching  (Send the command, then put your finger on sensor. "); Serial.write("Each time you send a command, module waits and matches once)\r\n"); 
	Serial.write(" CMD4 : Clear fingerprints\r\n"); 
	Serial.write(" CMD5 : Switch to sleep mode, you can use the finger Automatic wake-up function"); 
	Serial.write(" CMD6 : Wake up and make all commands valid\r\n");
	Serial.write("*************** WaveShare Capacitive Fingerprint Reader Test ***************\r\n");	
}

void Analysis_PC_Command(void)
{	
	static  uint8_t  step;
	uint8_t temp;
		
	if(Serial.available())
	{
		temp = Serial.read();
		
		switch(step)
		{
			case 0:
				if(temp == 'C')     step++;		
				else    step = 0;		
				break;		
			case 1:
				if(temp == 'M')     step++;		
				else    step = 0;
				break;				
			case 2:
				if(temp == 'D')     step++;		
				else    step = 0;
				break;				
			case 3:		
				switch(temp)
				{						
					case '1':
						if(Finger_SleepFlag == 1)  break;
						Serial.write("Number of fingerprints already available:  "); Serial.println(GetUserCount());
						break;			
					case '2':
						if(Finger_SleepFlag == 1)  break;
                        Serial.write(" Add fingerprint  (Each entry needs to be read two times: "); Serial.write("put the finger on sensor)\r\n"); 
						switch(AddUser())
						{
							case ACK_SUCCESS:
								Serial.println("Fingerprint added successfully !");
								break;
							
							case ACK_FAIL: 			
								Serial.println("Failed: Please try to place the center of the fingerprint flat to sensor, or this fingerprint already exists !");
								break;
							
							case ACK_FULL:			
								Serial.println("Failed: The fingerprint library is full !");
								break;		
						}
						break;					
					case '3':
						if(Finger_SleepFlag == 1)  break;
						Serial.println("Waiting Finger......Please try to place the center of the fingerprint flat to sensor !");
						switch(VerifyUser())
						{
							case ACK_SUCCESS:	
                Serial.print("CMD3-User: ");
                Serial.print(user_id); 
								Serial.println(" Matching successful !");
								break;
							case ACK_NO_USER:
								Serial.println("Failed: This fingerprint was not found in the library !");
								break;
							case ACK_TIMEOUT:	
								Serial.println("Failed: Time out !");
								break;	

						}
						break;				
					case '4':
						if(Finger_SleepFlag == 1)  break;
						ClearAllUser();
						Serial.println("All fingerprints have been cleared !");
						break;				
					case '5':
						if(Finger_SleepFlag == 1)  break;
						digitalWrite(Finger_RST_Pin , LOW);
						Finger_SleepFlag = 1;
           Serial1.end();
						Serial.println("Module has entered sleep mode: you can use the finger Automatic wake-up function, in this mode, only CMD6 is valid, send CMD6 to pull up the RST pin of module, so that the module exits sleep !");	
						break;
					case '6':					
						digitalWrite(Finger_RST_Pin , HIGH);
            Serial1.begin(19200);
						delay(300);  // Wait for module to start				
						Finger_SleepFlag = 0;							
						Serial.println("The module is awake. All commands are valid !");	
						break;
					
					default: break;
				}
							
				step = 0;				
				break;
				
			default:   break;	
		}
	}
}

void Auto_Verify_Finger(void)
{
  if(digitalRead(Finger_WAKE_Pin)!=LOW){  
			digitalWrite(Finger_RST_Pin , HIGH);    // Pull up the RST to start the module and start matching the fingers
			delay(1000); 
      Serial1.begin(19200, SERIAL_8N1, 16, 17);
      Serial.println("Please keep puting your finger on it");
        
			switch(VerifyUser())
			{       
				case ACK_SUCCESS:	
          Serial.println("You can leave your finger now");  
          delay(2000);
          Serial.print("Auto-User: ");
          Serial.print(user_id); 
					Serial.println(" Matching successful !");
					break;
				case ACK_NO_USER:
          Serial.println("You can leave your finger now");  
					Serial.println("Failed: This fingerprint was not found in the library !");
					break;
				case ACK_TIMEOUT:	
					Serial.println("Failed: Time out !");
					break;	
        default:
          Serial.println("Failed: error!");
          break;
     }
			//After the matching action is completed, drag RST down to sleep
			//and continue to wait for your fingers to press
			digitalWrite(Finger_RST_Pin , LOW);   
      
      Serial1.end();
      delay(500);
      }
}
