/* ------------------------------------------------------------------------ */
/**
 * \addtogroup e2forlife_w5500
 * @{
 */
/**
 * Copyright (c) 2015, E2ForLife.com
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of E2ForLife.com nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/**
 * \file socket.c
 * \author Chuck Erhardt (chuck@e2forlife.com)
 * 
 * This file contains the code used for manipulation of the sockets within the
 * WizNET device.  They call functions from the driver core, and are intended
 * to be able to support sever device variants. (note that low-level protocols
 * between WizNET devices are highly different).
 */
/* ======================================================================== */
#include <cytypes.h>
#include <cylib.h>

#include "`$INSTANCE_NAME`.h"

extern uint8 `$INSTANCE_NAME`_socketStatus[`$INSTANCE_NAME`_MAX_SOCKETS];

/* ------------------------------------------------------------------------ */
/**
 * \brief send a socket command
 * \param socket (uint8) the socket to command
 * \param cmd (uint8) the command to issue to the socket.
 *
 * This function writes teh command register then waits for the command
 * register to return a 0. then it returns the last read value.
 */
cystatus `$INSTANCE_NAME`_ExecuteSocketCommand(uint8 socket, uint8 cmd )
{
	uint8 result;
	uint32 timeout;
	
	if (socket >= `$INSTANCE_NAME`_MAX_SOCKETS) return CYRET_INVALID_OBJECT;
	
	`$INSTANCE_NAME`_Send(`$INSTANCE_NAME`_SREG_CR, `$INSTANCE_NAME`_SOCKET_BASE(socket), 1, &cmd, 1);
	timeout = 0;
	do {
		`$INSTANCE_NAME`_Send( `$INSTANCE_NAME`_SREG_CR, `$INSTANCE_NAME`_SOCKET_BASE(socket), 0, &result, 1);
		if (result != 0) {
			CyDelay(1);
			++timeout;
		}
	}
	while ( (result != 0) && (timeout < `$INSTANCE_NAME`_CMD_TIMEOUT) );
	
	result = (result == 0)? CYRET_SUCCESS : CYRET_TIMEOUT;
	return result;
}
/* ------------------------------------------------------------------------ */
/**
 * \brief Open and initialize a socket for use
 * \param port (uint16) the por tnumbe ron which the socket will be open
 * \param flags (uint8) Protocol and setting information for the socket mode
 * \returns uint8 socket handle 0 - 7
 * \returns 0xFF socket open error
 *
 * `$INSTANCE_NAME`_SocketOpen initializes and opens a socket on the specified port using
 * the socket mode specified by the flags.  Upon sucessful opening, the socket
 * handle will be returned for future use by the driver.  A return value of 
 * 0xFF indicates a socket open failure.
 *
 * Socket handle 0 is reserved for MACRAW operations, since socket 0 of the
 * W5500 is the only socket where MACRAW more operations are valid.
 *
 * \note the Tx_size and rx_size parameters are ignored and assumed to be 2K
 */
uint8 `$INSTANCE_NAME`_SocketOpen( uint16 port, uint8 flags)
{
	uint8 socket;
	int idx;
	
	/* default the socket to the error condition */
	socket = 0xFF;
	/* 
	 * If the MACRAW protocol was selected, make sure that socket 0 is
	 * available, otherwise flag an error and kick back to the user
	 */
	if ( (flags & `$INSTANCE_NAME`_PROTO_MACRAW) != 0) {
		if (`$INSTANCE_NAME`_socketStatus[0] != `$INSTANCE_NAME`_SOCKET_AVAILALE) {
			return 0xFF;
		}
		else {
			socket = 0;
		}
	}
	else {
		/*
		 * scan the other sockets, starting at 1 (0 is ignore to support the
		 * MACRAW mode of operation) and look for the first available socket.
		 * once there is a socket available, exit the loop and store the socket
		 * id in the socket number.
		 */
		idx = 1;
		while ( (idx < `$INSTANCE_NAME`_MAX_SOCKETS) && (`$INSTANCE_NAME`_socketStatus[idx] == `$INSTANCE_NAME`_SOCKET_OPEN) ) {
			++idx;
		}
		if ( idx >= `$INSTANCE_NAME`_MAX_SOCKETS ) {
			return 0xFF;
		}
		socket = idx;
	}
	/*
	 * Now that the socket is identified, declare it as open, then set the mode
	 * register and issue the socket open command
	 */
	`$INSTANCE_NAME`_socketStatus[socket] = `$INSTANCE_NAME`_SOCKET_OPEN;
	`$INSTANCE_NAME`_Send( `$INSTANCE_NAME`_SREG_MR, `$INSTANCE_NAME`_SOCKET_BASE(socket), 1, &flags, 1);
	/*
	 * Initialize memory pointers for the W5500 device
	 */
	port = CYSWAP_ENDIAN16(port);
	`$INSTANCE_NAME`_Send( `$INSTANCE_NAME`_SREG_PORT, `$INSTANCE_NAME`_SOCKET_BASE(socket), 1, (uint8*)&port, 2);
	/*
	 * execute the open command, and check the result.  If the result
	 * was not 0, there was an error in the command, so set the socket
	 * as available, and return 0xFF error socket to safely exit the error
	 * condition.
	 */
	if (`$INSTANCE_NAME`_ExecuteSocketCommand(`$INSTANCE_NAME`_CR_OPEN, socket) != 0) {
		`$INSTANCE_NAME`_socketStatus[socket] = `$INSTANCE_NAME`_SOCKET_AVAILALE;
		socket = 0xFF;
	}
	return socket;
}
/* ------------------------------------------------------------------------- */
/**
 * \brief Close an open socket
 * \param sock (uint8) the socket number to close
 *
 * _SocketClose closes an open socket connection optionally sending the
 * disconnection sequence.  After the command, the socket allocation is
 * cleared.
 *
 * \sa w500_SocketDisconnect
 */
cystatus `$INSTANCE_NAME`_SocketClose( uint8 sock, uint8 discon )
{
	cystatus status;
	uint8 ir;
	
	ir = 0xFF;
	
	/* Trap socket invalid handle errors */
	if (sock > 7) return CYRET_BAD_PARAM;
	/* Ignore close requests for sockets that are not open */
	if (`$INSTANCE_NAME`_socketStatus[sock] == `$INSTANCE_NAME`_SOCKET_AVAILALE) return CYRET_BAD_PARAM;
	
	/*
	 * first read the status of the socket from the W5500, and return with an
	 * error code if the socket is already closed.
	 */
	`$INSTANCE_NAME`_Send(`$INSTANCE_NAME`_SREG_SR, `$INSTANCE_NAME`_SOCKET_BASE(sock),0,&ir,1);
	if (ir == `$INSTANCE_NAME`_SR_CLOSED) {
		`$INSTANCE_NAME`_socketStatus[sock] = `$INSTANCE_NAME`_SOCKET_AVAILALE;
		return CYRET_CANCELED;
	}
	
	/*
	 * the socket was allocated and also in a state that is not already closed,
	 * so issue the close command to the device to terminate the connection
	 */
	if (discon != 0) {
		status = `$INSTANCE_NAME`_ExecuteSocketCommand( sock, `$INSTANCE_NAME`_CR_DISCON );
	}
	status = `$INSTANCE_NAME`_ExecuteSocketCommand( sock, `$INSTANCE_NAME`_CR_CLOSE );
	
	if (status == CYRET_SUCCESS ) {
		`$INSTANCE_NAME`_socketStatus[sock] = `$INSTANCE_NAME`_SOCKET_AVAILALE;
	}
	/*
	 * clear pending socket interrupts
	 */
	`$INSTANCE_NAME`_Send(`$INSTANCE_NAME`_SREG_IR, `$INSTANCE_NAME`_SOCKET_BASE(sock), 1, &ir, 1 );
	
	return status;
}
/* ------------------------------------------------------------------------ */
/**
 * \brief Close a socket and send the termination sequence to an open connection
 * \param sock (uint8) The socket number to colose
 * \returns 0 successful closure of the socket
 * \returns 0xFF socket was already closed, or not allocated
 * \retutns uint8 value indicating faulure of the discon command execution
 */
cystatus `$INSTANCE_NAME`_SocketDisconnect( uint8 sock )
{
	return `$INSTANCE_NAME`_SocketClose( sock, 1);
}
/* ------------------------------------------------------------------------ */
/**
 * \brief Check to see if a SEND operation was completed.
 */
cystatus `$INSTANCE_NAME`_SocketSendComplete( uint8 socket )
{
	uint8 ir;
	cystatus result;
	
	if (`$INSTANCE_NAME`_SOCKET_BAD( socket ) ) return CYRET_BAD_PARAM;

	/* Set the result to waiting to process */
	result = CYRET_STARTED;
	/* Read the Socket IR and check the flags for a send OK */
	`$INSTANCE_NAME`_Send(`$INSTANCE_NAME`_SREG_IR, `$INSTANCE_NAME`_SOCKET_BASE(socket),0,&ir,1);
	if ( (ir&`$INSTANCE_NAME`_IR_SEND_OK) == 0 ) {
		/*
		 * The Send was not complete, so now read the status to determine if the
		 * socket was closed remotely.
		 */
		`$INSTANCE_NAME`_Send(`$INSTANCE_NAME`_SREG_SR, `$INSTANCE_NAME`_SOCKET_BASE(socket),0,&ir,1);
		if ( ir == `$INSTANCE_NAME`_SR_CLOSED ) {
			`$INSTANCE_NAME`_SocketClose( socket, 1 );
			result = CYRET_CANCELED;
		}
	}
	else {
		result = CYRET_FINISHED;
	}
	
	return result;
}
/** @} */
/* [] END OF FILE */
