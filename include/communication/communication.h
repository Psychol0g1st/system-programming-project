#ifndef COMMUNICATION_H
#define COMMUNICATION_H
void SendViaFile(int *, int);
void ReceiveViaFile();
void SendViaSocket(int *, int);
void ReceiveViaSocket();
void SignalHandler(int);
#endif