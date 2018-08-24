#ifndef PC_DELWITH_HHJ
#define PC_DELWITH_HHJ

#define BAT_THRESHOLD   220 //����2.2V����

extern void SendConfig(uint8_t com);
extern void SendBack(uint8_t com, ECHO echoCode);
extern void SendBatteryVoltage(uint8_t com);
extern void SendBackValue(uint8_t com, uint32_t d);
extern uint8_t pcCheck(DEVICE com);

#endif






