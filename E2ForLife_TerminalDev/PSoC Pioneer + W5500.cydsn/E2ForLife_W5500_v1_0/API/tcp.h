/**
 * @addtogroup e2forlife_w5500
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
 * @file tcp.h
 * @author Chuck Erhardt (chuck@e2forlife.com)
 * 
 * Changes: Carlos Diaz (carlos.santiago.diaz@gmail.com) 2017
 * Header file for the tcp functions.
 */

#ifndef `$INSTANCE_NAME`_TCP_H
#define `$INSTANCE_NAME`_TCP_H

#include <cyfitter.h>
#include <cytypes.h>
#include <CyLib.h>

cystatus `$INSTANCE_NAME`_TcpConnected( uint8_t socket );
uint8_t `$INSTANCE_NAME`_TcpOpenClient( uint16_t port, uint32_t remote_ip, uint16_t remote_port );
uint8_t `$INSTANCE_NAME`_TcpOpenServer( uint16_t port );
cystatus `$INSTANCE_NAME`_TcpWaitForConnection( uint8_t socket );
uint16_t `$INSTANCE_NAME`_TcpSend( uint8_t socket, uint8_t* buffer, uint16_t len, uint8_t flags );
void `$INSTANCE_NAME`_TcpPrint( uint8_t socket, const char* string );
uint16_t `$INSTANCE_NAME`_TcpReceive( uint8_t socket, uint8_t* buffer, uint16_t len, uint8_t flags );
char `$INSTANCE_NAME`_TcpGetChar( uint8_t socket );
int `$INSTANCE_NAME`_TcpGetLine( uint8_t socket, char *buffer );

#endif /* `$INSTANCE_NAME`_TCP_H */
/* [] END OF FILE */
