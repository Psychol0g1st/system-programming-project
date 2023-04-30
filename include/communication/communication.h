#ifndef COMMUNICATION_H
#define COMMUNICATION_H
void SendViaFile(int *, int);
void ReceiveViaFile(int);
void SendViaSocket(int *, int);
void SignalHandler(int);
#endif