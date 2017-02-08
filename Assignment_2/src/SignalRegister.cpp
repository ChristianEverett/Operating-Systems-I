/*
 * SignalRegister.cpp
 *
 *  Created on: Sep 11, 2016
 *      Author: Christian Everett
 */

#include "SignalRegister.h"

SignalRegister::SignalRegister(int signals[], int length)
{
	//int length = (sizeof(signals)/sizeof(*signals));

	sigemptyset(&mask_sigs);

	for(int x = 0; x < length; x++)
	{
		this->signals.push_back(signals[x]);

		if(sigaddset(&mask_sigs, this->signals.at(x)) == -1)
		{
			throw std::runtime_error("invalid signal");
		}
	}
}

SignalRegister::~SignalRegister()
{
	if(sigprocmask(SIG_UNBLOCK, &mask_sigs, NULL) == -1)
	{
		throw std::runtime_error("invalid action on signal mask");
	}

	for(int x = 0; x < this->signals.size(); x++)
	{
		if(signal(this->signals.at(x), SIG_DFL) == SIG_ERR)
		{
			throw std::runtime_error("Can't change signal disposition for: " + this->signals.at(x));
		}
	}
}

void SignalRegister::setSignalHandler(void (*handler)(int))
{
	struct sigaction actionStruct;
	actionStruct.sa_mask = mask_sigs;
	actionStruct.sa_flags = 0;
	actionStruct.sa_handler = handler;

	for(int x = 0; x < this->signals.size(); x++)
	{
		if(sigaction(this->signals.at(x), &actionStruct, NULL) == -1)
		{
			throw std::runtime_error("Can't set handler for signal: " + this->signals.at(x));
		}
	}
}

void SignalRegister::blockAllOtherSignals()
{
	sigset_t block_sigs;

	sigfillset(&block_sigs);

	for(int x = 0;  x < signals.size(); x++)
	{
		sigdelset(&block_sigs, signals.at(x));
	}

	if(sigprocmask(SIG_BLOCK, &block_sigs, NULL) == -1)
	{
		throw std::runtime_error("invalid action on signal mask");
	}
}

void SignalRegister::blockSignals()
{
	if(sigprocmask(SIG_BLOCK, &mask_sigs, NULL) == -1)
	{
		throw std::runtime_error("invalid action on signal mask");
	}
}

void SignalRegister::unblockSignals()
{
	if(sigprocmask(SIG_UNBLOCK, &mask_sigs, NULL) == -1)
	{
		throw std::runtime_error("invalid action on signal mask");
	}
}

//void SignalRegister::registerSignals(int signal[])
//{
//
//}
