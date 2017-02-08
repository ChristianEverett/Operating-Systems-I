/*
 * SignalRegister.h
 *
 *  Created on: Sep 11, 2016
 *      Author: Christian Everett
 */

#ifndef SIGNALREGISTER_H_
#define SIGNALREGISTER_H_

#include <vector>
#include <signal.h>
#include <stdexcept>

class SignalRegister
{
public:
	SignalRegister(int signals[], int length);
	virtual ~SignalRegister();

	//void registerSignals(int signals[]);
	void unblockSignals();
	void blockSignals();
	void blockAllOtherSignals();
	void setSignalHandler(void (*handler)(int));

private:
	std::vector<int> signals;
	sigset_t mask_sigs;
};

#endif /* SIGNALREGISTER_H_ */
