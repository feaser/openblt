unit WSockets;
{

WSockets Version 1.20 - A Simple VCL Encapsulation of the WinSocket API

VCL Classes in this Unit:
  TTCPClient - A TCP Client (derived from TCustomWSocket)
  TTCPServer - A TCP Server (derived from TCustomWSocket)
  TUDPClient - A UDP Client (derived from TCustomWSocket)
  TUDPServer - A UDP Server (derived from TCustomWSocket)

Other classes ni this Unit:
  TCustomWSocket - A generic base class for other socket classes
  TClientList    - A list class used only by the TTCPServer class

Legal issues:

Copyright (C) 1997 by Robert T. Palmqvist <robert.palmqvist@skanska.se>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the author be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented, you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.

  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.

  3. This notice may not be removed or altered from any source distribution.

Credits go to:

  Gary T. Desrosiers. His unit "Sockets" inspired me to write my own.

  Martin Hall, Mark Towfig, Geoff Arnold, David Treadwell, Henry Sanders
  and InfoMagic, Inc. for their Windows Help File "WinSock.hlp".

  All the guys at Borland who gave us a marvellous tool named "Delphi"!

Recommended information sources:

  Specification:
    Windows Sockets Version 1.1 Specification

  Textbook:
    Quinn and Shute. "Windows Sockets Network Programming"
    1996 by Addison-Wesley Publishing Company, Inc. ISBN 0-201-63372-8

  World Wide Web:
    http://www.sockets.com
    http://www.stardust.com

  Network News:
    alt.winsock.programming

  Frequently Asked Questions:
    "WinSock Application FAQ" Mailto: info@lcs.com Subject: faq

  Requests for Comments:
    RFC 768 "User Datagram Protocol"
    RFC 791 "Internet Protocol"
    RFC 793 "Transmission Control Protocol"

}
interface

uses
  Windows, WinSock, SysUtils, Classes, Messages, Forms;

const
  WM_ASYNCSELECT = WM_USER + 1;
  READ_BUFFER_SIZE = 1024;
  MAX_LOOP = 100;

type
  TSocketState = (ssNotStarted, ssClosed, ssConnected, ssListening, ssOpen);

  TOnError = procedure(Sender: TObject; Error: integer; Msg: string) of object;
  TOnData = procedure(Sender: TObject; Socket: TSocket) of object;
  TOnAccept = procedure(Sender: TObject; Socket: TSocket) of object;
  TOnConnect = procedure(Sender: TObject; Socket: TSocket) of object;
  TOnClose = procedure(Sender: TObject; Socket: TSocket) of object;

  TReadBuffer = array[1..READ_BUFFER_SIZE] of byte;

  TClientList = class(TObject)
  private
    FSockets: TList;
  protected
    function GetSockets(Index: integer): TSocket;
    function GetCount: integer;
  public
    constructor Create;
    destructor Destroy; override;
    function Add(Socket: TSocket): boolean;
    procedure Delete(Socket: TSocket);
    procedure Clear;
    function IndexOf(Socket: TSocket): integer;
    property Sockets[Index: integer]: TSocket read GetSockets; default;
    property Count: integer read GetCount;
  end;

  TCustomWSocket = class(TComponent)
  private
    {WinSocket Information Private Fields}
    FVersion: string;
    FDescription: string;
    FSystemStatus: string;
    FMaxSockets: integer;
    FMaxUDPSize: integer;
    {End WinSocket Information Private Fields}
    FProtocol: integer;
    FType: integer;

    FReadBuffer: TReadBuffer;
    FLocalSocket: TSocket;
    FSocketState: TSocketState;
    FLastError: integer;
    FOnError: TOnError;
  protected
    procedure SocketError(Error: integer);
    function LastErrorDesc: string;

    function GetLocalHostAddress: string;
    function GetLocalHostName: string;
    {Socket Helper Functions}
    procedure SocketClose(var Socket: TSocket; Handle: HWND);
    function SocketQueueSize(Socket: TSocket): longint;

    procedure SocketWrite(Socket: TSocket; Flag: integer; Data: string);
    function SocketRead(Socket: TSocket; Flag: integer): string;
    function SocketWriteBuffer(Socket: TSocket; Buffer: Pointer; Size, Flag: integer): integer;
    function SocketReadBuffer(Socket: TSocket; Buffer: Pointer; Size, Flag: integer): integer;

    procedure SocketWriteTo(Socket: TSocket; Flag: integer; Data: string; var SockAddrIn: TSockAddrIn);
    function SocketReadFrom(Socket: TSocket; Flag: integer; var SockAddrIn: TSockAddrIn): string;
    function SocketWriteBufferTo(Socket: TSocket; Buffer: Pointer; Size, Flag: integer; var SockAddrIn: TSockAddrIn): integer;
    function SocketReadBufferFrom(Socket: TSocket; Buffer: Pointer; Size, Flag: integer; var SockAddrIn: TSockAddrIn): integer;
  public
    constructor Create(AOwner: TComponent); override;
    destructor Destroy; override;
    {Address and Port Resolving Helper Functions}
    function GetSockAddrIn(Host, Port: string; var SockAddrIn: TSockAddrIn): boolean;
    function GetAnySockAddrIn(Port: string; var SockAddrIn: TSockAddrIn): boolean;
    function GetBroadcastSockAddrIn(Port: string; var SockAddrIn: TSockAddrIn): boolean;
    function SockAddrInToName(SockAddrIn: TSockAddrIn): string;
    function SockAddrInToAddress(SockAddrIn: TSockAddrIn): string;
    function SockAddrInToPort(SockAddrIn: TSockAddrIn): string;
    function SocketToName(Socket: TSocket): string;
    function SocketToAddress(Socket: TSocket): string;
    function SocketToPort(Socket: TSocket): string;
    function PeerToName(Socket: TSocket): string;
    function PeerToAddress(Socket: TSocket): string;
    function PeerToPort(Socket: TSocket): string;
    {WinSocket Information Properties}
    property Version: string read FVersion;
    property Description: string read FDescription;
    property SystemStatus: string read FSystemStatus;
    property MaxSockets: integer read FMaxSockets;
    property MaxUDPSize: integer read FMaxUDPSize;
    {End WinSocket Information Properties}
    property LocalSocket: TSocket read FLocalSocket;
    property SocketState: TSocketState read FSocketState;
    property LastError: integer read FLastError;
    property LocalHostAddress: string read GetLocalHostAddress;
    property LocalHostName: string read GetLocalHostName;
  published
    property OnError: TOnError read FOnError write FOnError;
  end;

  TTCPClient = class(TCustomWSocket)
  private
    FHandle: HWND;

    FHost: string;
    FPort: string;

    FOnData: TOnData;
    FOnConnect: TOnConnect;
    FOnClose: TOnClose;
  protected
    procedure WndProc(var AMsg: TMessage);
    procedure OpenConnection(Socket: TSocket; Error: word);
    procedure IncommingData(Socket: TSocket; Error: word);
    procedure CloseConnection(Socket: TSocket; Error: word);

    function GetPeerAddress: string;
    function GetPeerPort: string;
  public
    constructor Create(AOwner: TComponent); override;
    destructor Destroy; override;

    procedure Open;
    procedure Close;

    function Peek: string;

    procedure Write(Data: string);
    function Read: string;

    function WriteBuffer(Buffer: Pointer; Size: integer): integer;
    function ReadBuffer(Buffer: Pointer; Size: integer): integer;

    property Handle: HWND read FHandle;

    property PeerAddress: string read GetPeerAddress;
    property PeerPort: string read GetPeerPort;
  published
    property Host: string read FHost write FHost;
    property Port: string read FPort write FPort;

    property OnData: TOnData read FOnData write FOnData;
    property OnConnect: TOnConnect read FOnConnect write FOnConnect;
    property OnClose: TOnClose read FOnClose write FOnClose;
  end;

  TTCPServer = class(TCustomWSocket)
  private
    FHandle: HWND;
    FPort: string;

    FOnData: TOnData;
    FOnAccept: TOnAccept;
    FOnClose: TOnClose;

    FClients: TClientList;
  protected
    procedure WndProc(var AMsg: TMessage);
    procedure OpenConnection(Socket: TSocket; Error: word);
    procedure IncommingData(Socket: TSocket; Error: word);
    procedure CloseConnection(Socket: TSocket; Error: word);
  public
    constructor Create(AOwner: TComponent); override;
    destructor Destroy; override;

    procedure Open;
    procedure Close;

    function Peek(Socket: TSocket): string;

    procedure Write(Socket: TSocket; Data: string);
    function Read(Socket: TSocket): string;

    function WriteBuffer(Socket: TSocket; Buffer: Pointer; Size: integer): integer;
    function ReadBuffer(Socket: TSocket; Buffer: Pointer; Size: integer): integer;

    procedure Disconnect(Socket: TSocket);

    property Handle: HWND read FHandle;
    property Clients: TClientList read FClients;
  published
    property Port: string read FPort write FPort;

    property OnData: TOnData read FOnData write FOnData;
    property OnAccept: TOnAccept read FOnAccept write FOnAccept;
    property OnClose: TOnClose read FOnClose write FOnClose;
  end;

  TUDPClient = class(TCustomWSocket)
  private
    FHandle: HWND;

    FHost: string;
    FPort: string;

    FOnData: TOnData;
  protected
    procedure WndProc(var AMsg: TMessage);
    procedure IncommingData(Socket: TSocket; Error: word);

    function GetPeerAddress: string;
    function GetPeerPort: string;
  public
    constructor Create(AOwner: TComponent); override;
    destructor Destroy; override;

    procedure Open;
    procedure Close;

    function Peek: string;

    procedure Write(Data: string);
    function Read: string;

    function WriteBuffer(Buffer: Pointer; Size: integer): integer;
    function ReadBuffer(Buffer: Pointer; Size: integer): integer;

    property Handle: HWND read FHandle;

    property PeerAddress: string read GetPeerAddress;
    property PeerPort: string read GetPeerPort;
  published
    property Host: string read FHost write FHost;
    property Port: string read FPort write FPort;

    property OnData: TOnData read FOnData write FOnData;
  end;

  TUDPServer = class(TCustomWSocket)
  private
    FHandle: HWND;
    FPort: string;

    FOnData: TOnData;
  protected
    procedure WndProc(var AMsg: TMessage);
    procedure IncommingData(Socket: TSocket; Error: word);
  public
    constructor Create(AOwner: TComponent); override;
    destructor Destroy; override;

    procedure Open;
    procedure Close;

    function Peek(Socket: TSocket; var SockAddrIn: TSockAddrIn): string;

    procedure Write(Socket: TSocket; Data: string; var SockAddrIn: TSockAddrIn);
    function Read(Socket: TSocket; var SockAddrIn: TSockAddrIn): string;

    function WriteBuffer(Socket: TSocket; Buffer: Pointer; Size: integer; var SockAddrIn: TSockAddrIn): integer;
    function ReadBuffer(Socket: TSocket; Buffer: Pointer; Size: integer; var SockAddrIn: TSockAddrIn): integer;

    property Handle: HWND read FHandle;
  published
    property Port: string read FPort write FPort;

    property OnData: TOnData read FOnData write FOnData;
  end;

procedure Register;

implementation

procedure Register;
begin
  RegisterComponents('Samples', [TTCPClient, TTCPServer, TUDPClient, TUDPServer]);
end;

(**** TClientList Class ****)

constructor TClientList.Create;
begin
  inherited Create;
  FSockets:= TList.Create;
end;

destructor TClientList.Destroy;
begin
  Clear;
  FSockets.Free;
  inherited Destroy;
end;

function TClientList.GetSockets(Index: integer): TSocket;
begin
  Result:= TSocket(FSockets[Index]);
end;

function TClientList.GetCount: integer;
begin
  Result:= FSockets.Count;
end;

function TClientList.Add(Socket: TSocket): boolean;
begin
  Result:= (FSockets.Add(Ptr(Socket)) >= 0);
end;

procedure TClientList.Delete(Socket: TSocket);
var
  i: integer;
begin
  for i:= 0 to FSockets.Count-1 do
    begin
      if TSocket(FSockets[i]) = Socket then
        begin
          FSockets.Delete(i);
          Break;
        end;
    end;
end;

procedure TClientList.Clear;
begin
  FSockets.Clear;
end;

function TClientList.IndexOf(Socket: TSocket): integer;
var
  i: integer;
begin
  Result:= -1;
  for i:= 0 to FSockets.Count-1 do
    begin
      if TSocket(FSockets[i]) = Socket then
        begin
          Result:= i;
          Break;
        end;
    end;
end;

(**** TCustomWSocket Class ****)

constructor TCustomWSocket.Create(AOwner: TComponent);
var
  WSAData: TWSAData;
begin
  inherited Create(AOwner);
  FProtocol:= IPPROTO_IP;
  FType:= SOCK_RAW;
  FLocalSocket:= INVALID_SOCKET;
  FSocketState:= ssNotStarted;
  FLastError:= WSAStartup($101, WSAData);
  if FLastError = 0 then
    begin
      FSocketState:= ssClosed;
      with WSAData do
        begin
          FVersion:= Concat(IntToStr(Hi(wVersion)),'.',(IntToStr(Lo(wVersion))));
          FDescription:= StrPas(szDescription);
          FSystemStatus:= StrPas(szSystemStatus);
          FMaxSockets:= iMaxSockets;
          FMaxUDPSize:= iMaxUDPDg;
        end;
    end
  else
    SocketError(FLastError);
end;

destructor TCustomWSocket.Destroy;
begin
  if FLocalSocket <> INVALID_SOCKET then
    closesocket(FLocalSocket);
  if FSocketState <> ssNotStarted then
    if WSACleanUp = SOCKET_ERROR then
      SocketError(WSAGetLastError);
  inherited Destroy;
end;

function TCustomWSocket.GetSockAddrIn(
         Host, Port: string; var SockAddrIn: TSockAddrIn): boolean;
var
  ProtoEnt: PProtoEnt;
  ServEnt: PServEnt;
  HostEnt: PHostEnt;
begin
  Result:= false;
  SockAddrIn.sin_family:= AF_INET;

  ProtoEnt:= getprotobynumber(FProtocol);
  if ProtoEnt = nil then
    begin
      SocketError(WSAGetLastError);
      Exit;
    end;

  ServEnt:= getservbyname(PChar(Port), ProtoEnt^.p_name);
  if ServEnt = nil then
    SockAddrIn.sin_port:= htons(StrToInt(Port))
  else
    SockAddrIn.sin_port:= ServEnt^.s_port;

  SockAddrIn.sin_addr.s_addr:= inet_addr(PChar(Host));
  if SockAddrIn.sin_addr.s_addr = INADDR_NONE then
    begin
      HostEnt:= gethostbyname(PChar(Host));
      if HostEnt = nil then
        begin
         SocketError(WSAGetLastError);
         Exit;
        end;
      SockAddrIn.sin_addr.S_addr:= longint(plongint(HostEnt^.h_addr_list^)^);
    end;
  Result:= true;
end;

function TCustomWSocket.GetAnySockAddrIn(
         Port: string; var SockAddrIn: TSockAddrIn): boolean;
var
  ProtoEnt: PProtoEnt;
  ServEnt: PServEnt;
begin
  Result:= false;
  SockAddrIn.sin_family:= AF_INET;

  ProtoEnt:= getprotobynumber(FProtocol);
  if ProtoEnt = nil then
    Exit;

  ServEnt:= getservbyname(PChar(Port), ProtoEnt^.p_name);
  if ServEnt = nil then
    SockAddrIn.sin_port:= htons(StrToInt(Port))
  else
    SockAddrIn.sin_port:= ServEnt^.s_port;

  SockAddrIn.sin_addr.s_addr:= INADDR_ANY;
  Result:= true;
end;

function TCustomWSocket.GetBroadcastSockAddrIn(
         Port: string; var SockAddrIn: TSockAddrIn): boolean;
var
  ProtoEnt: PProtoEnt;
  ServEnt: PServEnt;
begin
  Result:= false;
  SockAddrIn.sin_family:= AF_INET;

  ProtoEnt:= getprotobynumber(FProtocol);
  if ProtoEnt = nil then
    Exit;

  ServEnt:= getservbyname(PChar(Port), ProtoEnt^.p_name);
  if ServEnt = nil then
    SockAddrIn.sin_port:= htons(StrToInt(Port))
  else
    SockAddrIn.sin_port:= ServEnt^.s_port;

  SockAddrIn.sin_addr.s_addr:= INADDR_BROADCAST;
  Result:= true;
end;

function TCustomWSocket.SockAddrInToName(SockAddrIn: TSockAddrIn): string;
var
  HostEnt: PHostEnt;
begin
  HostEnt:= gethostbyaddr(@SockAddrIn.sin_addr.S_addr, 4, AF_INET);
  if HostEnt <> nil then
    Result:= HostEnt.h_name;
end;

function TCustomWSocket.SockAddrInToAddress(SockAddrIn: TSockAddrIn): string;
begin
  Result:= inet_ntoa(SockAddrIn.sin_addr);
end;

function TCustomWSocket.SockAddrInToPort(SockAddrIn: TSockAddrIn): string;
begin
  Result:= IntToStr(ntohs(SockAddrIn.sin_port));
end;

function TCustomWSocket.SocketToName(Socket: TSocket): string;
var
  SockAddrIn: TSockAddrIn;
  Len: integer;
  HostEnt: PHostEnt;
begin
  if Socket <> INVALID_SOCKET then
    begin
      Len:= SizeOf(SockAddrIn);
      if getsockname(Socket, SockAddrIn, Len) <> SOCKET_ERROR then
        begin
          HostEnt:= gethostbyaddr(@SockAddrIn.sin_addr.S_addr, 4, AF_INET);
          if HostEnt <> nil then
            Result:= HostEnt.h_name;
        end;
    end;
end;

function TCustomWSocket.SocketToAddress(Socket: TSocket): string;
var
  SockAddrIn: TSockAddrIn;
  Len: integer;
begin
  if Socket <> INVALID_SOCKET then
    begin
      Len:= SizeOf(SockAddrIn);
      if getsockname(Socket, SockAddrIn, Len) <> SOCKET_ERROR then
        Result:= inet_ntoa(SockAddrIn.sin_addr);
    end;
end;

function TCustomWSocket.SocketToPort(Socket: TSocket): string;
var
  SockAddrIn: TSockAddrIn;
  Len: integer;
begin
  if Socket <> INVALID_SOCKET then
    begin
      Len:= SizeOf(SockAddrIn);
      if getsockname(Socket, SockAddrIn, Len) <> SOCKET_ERROR then
        Result:= IntToStr(ntohs(SockAddrIn.sin_port));
    end;
end;

function TCustomWSocket.PeerToName(Socket: TSocket): string;
var
  SockAddrIn: TSockAddrIn;
  Len: integer;
  HostEnt: PHostEnt;
begin
  if Socket <> INVALID_SOCKET then
    begin
      Len:= SizeOf(SockAddrIn);
      if getpeername(Socket, SockAddrIn, Len) <> SOCKET_ERROR then
        begin
          HostEnt:= gethostbyaddr(@SockAddrIn.sin_addr.S_addr, 4, AF_INET);
          if HostEnt <> nil then
            Result:= HostEnt.h_name;
        end;
    end;
end;

function TCustomWSocket.PeerToAddress(Socket: TSocket): string;
var
  SockAddrIn: TSockAddrIn;
  Len: integer;
begin
  if Socket <> INVALID_SOCKET then
    begin
      Len:= SizeOf(SockAddrIn);
      if getpeername(Socket, SockAddrIn, Len) <> SOCKET_ERROR then
        Result:= inet_ntoa(SockAddrIn.sin_addr);
    end;
end;

function TCustomWSocket.PeerToPort(Socket: TSocket): string;
var
  SockAddrIn: TSockAddrIn;
  Len: integer;
begin
  if Socket <> INVALID_SOCKET then
    begin
      Len:= SizeOf(SockAddrIn);
      if getpeername(Socket, SockAddrIn, Len) <> SOCKET_ERROR then
        Result:= IntToStr(ntohs(SockAddrIn.sin_port));
    end;
end;

procedure TCustomWSocket.SocketClose(var Socket: TSocket; Handle: HWND);
var
  RC: integer;
begin
  if Socket <> INVALID_SOCKET then
    begin
      if WSAASyncSelect(Socket, Handle, WM_ASYNCSELECT, 0) <> 0 then
        begin
          SocketError(WSAGetLastError);
          Exit;
        end;

      if shutdown(Socket, 1) <> 0 then
        if WSAGetLastError <> WSAENOTCONN then
          begin
            SocketError(WSAGetLastError);
            Exit;
          end;

      repeat
        RC:= recv(Socket, FReadBuffer, SizeOf(TReadBuffer), 0);
      until (RC = 0) or (RC = SOCKET_ERROR);

      if closesocket(Socket) <> 0 then
        SocketError(WSAGetLastError)
      else
        Socket:= INVALID_SOCKET;
    end;
end;

function TCustomWSocket.SocketQueueSize(Socket: TSocket): longint;
var
  Size: longint;
begin
  Result:= 0;
  if ioctlsocket(Socket, FIONREAD, Size) <> 0 then
    SocketError(WSAGetLastError)
  else
    Result:= Size;
end;

procedure TCustomWSocket.SocketWrite(Socket: TSocket; Flag: integer; Data: string);
var
  TotSent, ToSend, Sent, ErrorLoop: integer;
begin
  if Data <> '' then
    begin
      ErrorLoop:= 0;
      TotSent:= 0;
      ToSend:= Length(Data);
      repeat
        Sent:= send(Socket, Data[TotSent+1], (ToSend-TotSent), Flag);
        if Sent = SOCKET_ERROR then
          begin
            Inc(ErrorLoop);
            if WSAGetLastError <> WSAEWOULDBLOCK then
              begin
                SocketError(WSAGetLastError);
                Exit;
              end;
          end
        else
          Inc(TotSent, Sent);
      until (TotSent >= ToSend) or (ErrorLoop > MAX_LOOP);
    end;
end;

function TCustomWSocket.SocketRead(Socket: TSocket; Flag: integer): string;
var
  Received: longint;
begin
  Result:= '';
  Received:= recv(Socket, FReadBuffer, SizeOf(TReadBuffer), Flag);
  if Received = SOCKET_ERROR then
    begin
      if WSAGetLastError <> WSAEWOULDBLOCK then
        SocketError(WSAGetLastError);
    end
  else
    begin
      SetLength(Result, Received);
      Move(FReadBuffer, Result[1], Received);
    end;
end;

function TCustomWSocket.SocketWriteBuffer(Socket: TSocket; Buffer: Pointer; Size, Flag: integer): integer;
begin
  Result:= send(Socket, Buffer^, Size, Flag);
  if Result = SOCKET_ERROR then
    begin
      Result:= 0;
      if WSAGetLastError <> WSAEWOULDBLOCK then
        SocketError(WSAGetLastError);
    end;
end;

function TCustomWSocket.SocketReadBuffer(Socket: TSocket; Buffer: Pointer; Size, Flag: integer): integer;
begin
  Result:= recv(Socket, Buffer^, Size, Flag);
  if Result = SOCKET_ERROR then
    begin
      Result:= 0;
      if WSAGetLastError <> WSAEWOULDBLOCK then
        SocketError(WSAGetLastError);
    end;
end;

procedure TCustomWSocket.SocketWriteTo(Socket: TSocket; Flag: integer; Data: string; var SockAddrIn: TSockAddrIn);
var
  TotSent, ToSend, Sent, ErrorLoop: integer;
begin
  if Data <> '' then
    begin
      ErrorLoop:= 0;
      TotSent:= 0;
      ToSend:= Length(Data);
      repeat
        Sent:= sendto(Socket, Data[TotSent+1], (ToSend-TotSent), Flag, SockAddrIn, SizeOf(SockAddrIn));
        if Sent = SOCKET_ERROR then
          begin
            Inc(ErrorLoop);
            if WSAGetLastError <> WSAEWOULDBLOCK then
              begin
                SocketError(WSAGetLastError);
                Exit;
              end;
          end
        else
          Inc(TotSent, Sent);
      until (TotSent >= ToSend) or (ErrorLoop > MAX_LOOP);
    end;
end;

function TCustomWSocket.SocketReadFrom(Socket: TSocket; Flag: integer; var SockAddrIn: TSockAddrIn): string;
var
  Len: integer;
  Received: longint;
begin
  Len:= SizeOf(SockAddrIn);
  Received:= recvfrom(Socket, FReadBuffer, SizeOf(TReadBuffer), Flag, SockAddrIn, Len);
  if Received = SOCKET_ERROR then
    begin
      if WSAGetLastError <> WSAEWOULDBLOCK then
        SocketError(WSAGetLastError);
    end
  else
    begin
      SetLength(Result, Received);
      Move(FReadBuffer, Result[1], Received);
    end;
end;

function TCustomWSocket.SocketWriteBufferTo(Socket: TSocket; Buffer: Pointer; Size, Flag: integer; var SockAddrIn: TSockAddrIn): integer;
begin
  Result:= sendto(Socket, Buffer^, Size, Flag, SockAddrIn, SizeOf(SockAddrIn));
  if Result = SOCKET_ERROR then
    begin
      Result:= 0;
      if WSAGetLastError <> WSAEWOULDBLOCK then
        SocketError(WSAGetLastError);
    end;
end;

function TCustomWSocket.SocketReadBufferFrom(Socket: TSocket; Buffer: Pointer; Size, Flag: integer; var SockAddrIn: TSockAddrIn): integer;
var
  Len: integer;
begin
  Len:= SizeOf(SockAddrIn);
  Result:= recvfrom(Socket, Buffer^, Size, Flag, SockAddrIn, Len);
  if Result = SOCKET_ERROR then
    begin
      Result:= 0;
      if WSAGetLastError <> WSAEWOULDBLOCK then
        SocketError(WSAGetLastError);
    end;
end;

procedure TCustomWSocket.SocketError(Error: integer);
begin
  FLastError:= Error;
  if Assigned(FOnError) then
    FOnError(Self, FLastError, LastErrorDesc);
end;

function TCustomWSocket.LastErrorDesc: string;
begin
  case FLastError of
    WSAEINTR           : Result:= 'Interrupted system call';
    WSAEBADF           : Result:= 'Bad file number';
    WSAEACCES          : Result:= 'Permission denied';
    WSAEFAULT          : Result:= 'Bad address';
    WSAEINVAL          : Result:= 'Invalid argument';
    WSAEMFILE          : Result:= 'Too many open files';
    WSAEWOULDBLOCK     : Result:= 'Operation would block';
    WSAEINPROGRESS     : Result:= 'Operation now in progress';
    WSAEALREADY        : Result:= 'Operation already in progress';
    WSAENOTSOCK        : Result:= 'Socket operation on nonsocket';
    WSAEDESTADDRREQ    : Result:= 'Destination address required';
    WSAEMSGSIZE        : Result:= 'Message too long';
    WSAEPROTOTYPE      : Result:= 'Protocol wrong type for socket';
    WSAENOPROTOOPT     : Result:= 'Protocol not available';
    WSAEPROTONOSUPPORT : Result:= 'Protocol not supported';
    WSAESOCKTNOSUPPORT : Result:= 'Socket not supported';
    WSAEOPNOTSUPP      : Result:= 'Operation not supported on socket';
    WSAEPFNOSUPPORT    : Result:= 'Protocol family not supported';
    WSAEAFNOSUPPORT    : Result:= 'Address family not supported';
    WSAEADDRINUSE      : Result:= 'Address already in use';
    WSAEADDRNOTAVAIL   : Result:= 'Can''t assign requested address';
    WSAENETDOWN        : Result:= 'Network is down';
    WSAENETUNREACH     : Result:= 'Network is unreachable';
    WSAENETRESET       : Result:= 'Network dropped connection on reset';
    WSAECONNABORTED    : Result:= 'Software caused connection abort';
    WSAECONNRESET      : Result:= 'Connection reset by peer';
    WSAENOBUFS         : Result:= 'No buffer space available';
    WSAEISCONN         : Result:= 'Socket is already connected';
    WSAENOTCONN        : Result:= 'Socket is not connected';
    WSAESHUTDOWN       : Result:= 'Can''t send after socket shutdown';
    WSAETOOMANYREFS    : Result:= 'Too many references:can''t splice';
    WSAETIMEDOUT       : Result:= 'Connection timed out';
    WSAECONNREFUSED    : Result:= 'Connection refused';
    WSAELOOP           : Result:= 'Too many levels of symbolic links';
    WSAENAMETOOLONG    : Result:= 'File name is too long';
    WSAEHOSTDOWN       : Result:= 'Host is down';
    WSAEHOSTUNREACH    : Result:= 'No route to host';
    WSAENOTEMPTY       : Result:= 'Directory is not empty';
    WSAEPROCLIM        : Result:= 'Too many processes';
    WSAEUSERS          : Result:= 'Too many users';
    WSAEDQUOT          : Result:= 'Disk quota exceeded';
    WSAESTALE          : Result:= 'Stale NFS file handle';
    WSAEREMOTE         : Result:= 'Too many levels of remote in path';
    WSASYSNOTREADY     : Result:= 'Network subsystem is unusable';
    WSAVERNOTSUPPORTED : Result:= 'Winsock DLL cannot support this application';
    WSANOTINITIALISED  : Result:= 'Winsock not initialized';
    WSAHOST_NOT_FOUND  : Result:= 'Host not found';
    WSATRY_AGAIN       : Result:= 'Non authoritative - host not found';
    WSANO_RECOVERY     : Result:= 'Non recoverable error';
    WSANO_DATA         : Result:= 'Valid name, no data record of requested type'
  else
    Result:= 'Not a Winsock error';
  end;
end;

function TCustomWSocket.GetLocalHostAddress: string;
var
  SockAddrIn: TSockAddrIn;
  HostEnt: PHostEnt;
  szHostName: array[0..128] of char;
begin
  if gethostname(szHostName, 128) = 0 then
    begin
      HostEnt:= gethostbyname(szHostName);
      if HostEnt = nil then
        Result:= ''
      else
        begin
          SockAddrIn.sin_addr.S_addr:= longint(plongint(HostEnt^.h_addr_list^)^);
          Result:= inet_ntoa(SockAddrIn.sin_addr);
        end;
    end
  else
    SocketError(WSAGetLastError);
end;

function TCustomWSocket.GetLocalHostName: string;
var
  szHostName: array[0..128] of char;
begin
  if gethostname(szHostName, 128) = 0 then
    Result:= szHostName
  else
    SocketError(WSAGetLastError);
end;

(**** TTCPClient Class ****)

constructor TTCPClient.Create(AOwner: TComponent);
begin
  inherited Create(AOwner);
  FHandle:= AllocateHWnd(WndProc);
  FProtocol:= IPPROTO_TCP;
  FType:= SOCK_STREAM;
end;

destructor TTCPClient.Destroy;
begin
  Close;
  DeallocateHWnd(FHandle);
  inherited Destroy;
end;

procedure TTCPClient.OpenConnection(Socket: TSocket; Error: word);
var
  EventMask: longint;
begin
  if Error <> 0 then
    SocketError(Error)
  else
    begin
      EventMask:= FD_READ or FD_CLOSE;
      if WSAASyncSelect(FLocalSocket, FHandle, WM_ASYNCSELECT, EventMask) <> 0 then
        SocketError(WSAGetLastError)
      else
        begin
          if Assigned(FOnConnect) then
            FOnConnect(Self, Socket);
          FSocketState:= ssConnected;
        end;
    end;
end;

procedure TTCPClient.CloseConnection(Socket: TSocket; Error: word);
begin
  if Error = WSAENETDOWN then
    SocketError(Error)
  else
    begin
      if Assigned(FOnClose) then
        FOnClose(Self, Socket);
      Close;
    end;
end;

procedure TTCPClient.IncommingData(Socket: TSocket; Error: word);
begin
  if Error <> 0 then
    SocketError(Error)
  else
    if Assigned(FOnData) then
      FOnData(Self, Socket);
end;

procedure TTCPClient.WndProc(var AMsg: TMessage);
var
  Error: word;
begin
  with AMsg do
    case Msg of
      WM_ASYNCSELECT:
        begin
          if (FSocketState = ssClosed) then
            Exit;
          Error:= WSAGetSelectError(LParam);
          case WSAGetSelectEvent(LParam) of
            FD_READ   : IncommingData(WParam, Error);
            FD_CONNECT: OpenConnection(WParam, Error);
            FD_CLOSE  : CloseConnection(WParam, Error);
          else
            if Error <> 0 then
              SocketError(Error);
          end;
        end;
    else
      Result:= DefWindowProc(FHandle, Msg, WParam, LParam);
    end;
end;

procedure TTCPClient.Open;
var
  SockAddrIn: TSockAddrIn;
  SockOpt: LongBool;
  EventMask: longint;
begin
  if (FSocketState <> ssClosed) then
    Exit;

  if not GetSockAddrIn(FHost, FPort, SockAddrIn) then
    Exit;

  FLocalSocket:= socket(PF_INET, FType, 0);
  if FLocalSocket = INVALID_SOCKET then
    begin
      SocketError(WSAGetLastError);
      Exit;
    end;

  EventMask:= (FD_CONNECT or FD_READ or FD_CLOSE);
  if WSAASyncSelect(FLocalSocket, FHandle, WM_ASYNCSELECT, EventMask) <> 0 then
    begin
      SocketError(WSAGetLastError);
      closesocket(FLocalSocket);
      Exit;
    end;

  SockOpt:= true; {Enable OOB Data inline}
  if setsockopt(FLocalSocket, SOL_SOCKET, SO_OOBINLINE, PChar(@SockOpt), SizeOf(SockOpt)) <> 0 then
    begin
      SocketError(WSAGetLastError);
      closesocket(FLocalSocket);
      Exit;
    end;

  if connect(FLocalSocket, SockAddrIn, SizeOf(SockAddrIn)) <> 0 then
    begin
      if WSAGetLastError <> WSAEWOULDBLOCK then
        begin
          SocketError(WSAGetLastError);
          closesocket(FLocalSocket);
          Exit;
        end;
    end;

  FSocketState:= ssOpen;
end;

procedure TTCPClient.Close;
begin
  if (FSocketState = ssNotStarted) or (FSocketState = ssClosed) then
    Exit;

  SocketClose(FLocalSocket, FHandle);
  if FLocalSocket = INVALID_SOCKET then
    FSocketState:= ssClosed;
end;

procedure TTCPClient.Write(Data: string);
begin
  SocketWrite(FLocalSocket, 0, Data);
end;

function TTCPClient.Read: string;
begin
  Result:= SocketRead(FLocalSocket, 0);
end;

function TTCPClient.Peek: string;
begin
  Result:= SocketRead(FLocalSocket, MSG_PEEK);
end;

function TTCPClient.WriteBuffer(Buffer: Pointer; Size: integer): integer;
begin
  Result:= SocketWriteBuffer(FLocalSocket, Buffer, Size, 0);
end;

function TTCPClient.ReadBuffer(Buffer: Pointer; Size: integer): integer;
begin
  Result:= SocketReadBuffer(FLocalSocket, Buffer, Size, 0);
end;

function TTCPClient.GetPeerAddress: string;
begin
  Result:= PeerToAddress(FLocalSocket);
end;

function TTCPClient.GetPeerPort: string;
begin
  Result:= PeerToPort(FLocalSocket);
end;

(**** TTCPServer Class ****)

constructor TTCPServer.Create(AOwner: TComponent);
begin
  inherited Create(AOwner);
  FHandle:= AllocateHWnd(WndProc);
  FProtocol:= IPPROTO_TCP;
  FType:= SOCK_STREAM;
  FClients:= TClientList.Create;
end;

destructor TTCPServer.Destroy;
begin
  Close;
  DeallocateHWnd(FHandle);
  FClients.Free;
  inherited Destroy;
end;

procedure TTCPServer.OpenConnection(Socket: TSocket; Error: word);
var
  Len: integer;
  NewSocket: TSocket;
  SockAddrIn: TSockAddrIn;
  SockOpt: LongBool;
  EventMask: longint;
begin
  if Error <> 0 then
    SocketError(Error)
  else
    begin
      Len:= SizeOf(SockAddrIn);
      //{$IFDEF VER100} // Delphi 3
      NewSocket:= accept(FLocalSocket, @SockAddrIn, @Len);
      //{$ELSE}         // Delphi 2
      //NewSocket:= accept(FLocalSocket, SockAddrIn, Len);
      //{$ENDIF}
      if NewSocket = INVALID_SOCKET then
        begin
          SocketError(WSAGetLastError);
          Exit;
        end;

      EventMask:= (FD_READ or FD_CLOSE);
      if WSAASyncSelect(NewSocket, FHandle, WM_ASYNCSELECT, EventMask) <> 0 then
        begin
          SocketError(WSAGetLastError);
          closesocket(NewSocket);
          Exit;
        end;

      SockOpt:= true; {Enable OOB Data inline}
      if setsockopt(NewSocket, SOL_SOCKET, SO_OOBINLINE	, PChar(@SockOpt), SizeOf(SockOpt)) <> 0 then
        begin
          SocketError(WSAGetLastError);
          closesocket(NewSocket);
          Exit;
        end;

      if not FClients.Add(NewSocket) then
        SocketClose(NewSocket, FHandle)
      else
        if Assigned(FOnAccept) then
          FOnAccept(Self, NewSocket);
    end;
end;

procedure TTCPServer.CloseConnection(Socket: TSocket; Error: word);
begin
  if Error = WSAENETDOWN then
    SocketError(Error)
  else
    begin
      if Assigned(FOnClose) then
        FOnClose(Self, Socket);
      Disconnect(Socket);
    end;
end;

procedure TTCPServer.IncommingData(Socket: TSocket; Error: word);
begin
  if Error <> 0 then
    SocketError(Error)
  else
    if Assigned(FOnData) then
      FOnData(Self, Socket);
end;

procedure TTCPServer.WndProc(var AMsg: TMessage);
var
  Error: word;
begin
  with AMsg do
    case Msg of
      WM_ASYNCSELECT:
        begin
          if (FSocketState = ssClosed) then
            Exit;
          Error:= WSAGetSelectError(LParam);
          case WSAGetSelectEvent(LParam) of
            FD_READ  : IncommingData(WParam, Error);
            FD_ACCEPT: OpenConnection(WParam, Error);
            FD_CLOSE : CloseConnection(WParam, Error);
          else
            if Error <> 0 then
              SocketError(Error);
          end;
        end;
    else
      Result:= DefWindowProc(FHandle, Msg, WParam, LParam);
    end;
end;

procedure TTCPServer.Open;
var
  SockAddrIn: TSockAddrIn;
begin
  if (FSocketState <> ssClosed) then
    Exit;

  if not GetAnySockAddrIn(FPort, SockAddrIn) then
    Exit;

  FLocalSocket:= socket(PF_INET, FType, 0);
  if FLocalSocket = INVALID_SOCKET then
    begin
      SocketError(WSAGetLastError);
      Exit;
    end;

  if WSAASyncSelect(FLocalSocket, FHandle, WM_ASYNCSELECT, FD_ACCEPT) <> 0 then
    begin
      SocketError(WSAGetLastError);
      closesocket(FLocalSocket);
      Exit;
    end;

  if bind(FLocalSocket, SockAddrIn, SizeOf(SockAddrIn)) <> 0 then
    begin
      SocketError(WSAGetLastError);
      closesocket(FLocalSocket);
      Exit;
    end;

  if listen(FLocalSocket, 5) <> 0 then
    begin
      SocketError(WSAGetLastError);
      closesocket(FLocalSocket);
      Exit;
    end;

  FSocketState:= ssListening;
end;

procedure TTCPServer.Close;
var
  i: integer;
  Dummy: TSocket;
begin
  if (FSocketState = ssNotStarted) or (FSocketState = ssClosed) then
    Exit;

  for i:= 0 to FClients.Count-1 do
    begin
      Dummy:= FClients[i];
      SocketClose(Dummy, FHandle);
    end;
  FClients.Clear;

  SocketClose(FLocalSocket, FHandle);
  if FLocalSocket = INVALID_SOCKET then
    FSocketState:= ssClosed;
end;

procedure TTCPServer.Write(Socket: TSocket; Data: string);
begin
  SocketWrite(Socket, 0, Data);
end;

function TTCPServer.Read(Socket: TSocket): string;
begin
  Result:= SocketRead(Socket, 0);
end;

function TTCPServer.Peek(Socket: TSocket): string;
begin
  Result:= SocketRead(Socket, MSG_PEEK);
end;

function TTCPServer.WriteBuffer(Socket: TSocket; Buffer: Pointer; Size: integer): integer;
begin
  Result:= SocketWriteBuffer(Socket, Buffer, Size, 0);
end;

function TTCPServer.ReadBuffer(Socket: TSocket; Buffer: Pointer; Size: integer): integer;
begin
  Result:= SocketReadBuffer(Socket, Buffer, Size, 0);
end;

procedure TTCPServer.Disconnect(Socket: TSocket);
begin
  FClients.Delete(Socket);
  SocketClose(Socket, FHandle);
end;

(**** TUDPClient Class ****)

constructor TUDPClient.Create(AOwner: TComponent);
begin
  inherited Create(AOwner);
  FHandle:= AllocateHWnd(WndProc);
  FProtocol:= IPPROTO_UDP;
  FType:= SOCK_DGRAM;
end;

destructor TUDPClient.Destroy;
begin
  Close;
  DeallocateHWnd(FHandle);
  inherited Destroy;
end;

procedure TUDPClient.IncommingData(Socket: TSocket; Error: word);
begin
  if Error <> 0 then
    SocketError(Error)
  else
    if Assigned(FOnData) then
      FOnData(Self, Socket);
end;

procedure TUDPClient.WndProc(var AMsg: TMessage);
var
  Error: word;
begin
  with AMsg do
    case Msg of
      WM_ASYNCSELECT:
        begin
          if (FSocketState = ssClosed) then
            Exit;
          Error:= WSAGetSelectError(LParam);
          case WSAGetSelectEvent(LParam) of
            FD_READ   : IncommingData(WParam, Error);
          else
            if Error <> 0 then
              SocketError(Error);
          end;
        end;
    else
      Result:= DefWindowProc(FHandle, Msg, WParam, LParam);
    end;
end;

procedure TUDPClient.Open;
var
  SockAddrIn: TSockAddrIn;
begin
  if (FSocketState <> ssClosed) then
    Exit;

  if not GetSockAddrIn(FHost, FPort, SockAddrIn) then
    Exit;

  FLocalSocket:= socket(PF_INET, FType, 0);
  if FLocalSocket = INVALID_SOCKET then
    begin
      SocketError(WSAGetLastError);
      Exit;
    end;

  if WSAASyncSelect(FLocalSocket, FHandle, WM_ASYNCSELECT, FD_READ) <> 0 then
    begin
      SocketError(WSAGetLastError);
      closesocket(FLocalSocket);
      Exit;
    end;

  if connect(FLocalSocket, SockAddrIn, SizeOf(SockAddrIn)) <> 0 then
    begin
      if WSAGetLastError <> WSAEWOULDBLOCK then
        begin
          SocketError(WSAGetLastError);
          closesocket(FLocalSocket);
          Exit;
        end;
    end;

  FSocketState:= ssOpen;
end;

procedure TUDPClient.Close;
begin
  if (FSocketState = ssNotStarted) or (FSocketState = ssClosed) then
    Exit;

  SocketClose(FLocalSocket, FHandle);
  if FLocalSocket = INVALID_SOCKET then
    FSocketState:= ssClosed;
end;

procedure TUDPClient.Write(Data: string);
begin
  SocketWrite(FLocalSocket, 0, Data);
end;

function TUDPClient.Read: string;
begin
  Result:= SocketRead(FLocalSocket, 0);
end;

function TUDPClient.Peek: string;
begin
  Result:= SocketRead(FLocalSocket, MSG_PEEK);
end;

function TUDPClient.WriteBuffer(Buffer: Pointer; Size: integer): integer;
begin
  Result:= SocketWriteBuffer(FLocalSocket, Buffer, Size, 0);
end;

function TUDPClient.ReadBuffer(Buffer: Pointer; Size: integer): integer;
begin
  Result:= SocketReadBuffer(FLocalSocket, Buffer, Size, 0);
end;

function TUDPClient.GetPeerAddress: string;
begin
  Result:= PeerToAddress(FLocalSocket);
end;

function TUDPClient.GetPeerPort: string;
begin
  Result:= PeerToPort(FLocalSocket);
end;

(**** TUDPServer Class ****)

constructor TUDPServer.Create(AOwner: TComponent);
begin
  inherited Create(AOwner);
  FHandle:= AllocateHWnd(WndProc);
  FProtocol:= IPPROTO_UDP;
  FType:= SOCK_DGRAM;
end;

destructor TUDPServer.Destroy;
begin
  Close;
  DeallocateHWnd(FHandle);
  inherited Destroy;
end;

procedure TUDPServer.IncommingData(Socket: TSocket; Error: word);
begin
  if Error <> 0 then
    SocketError(Error)
  else
    if Assigned(FOnData) then
      FOnData(Self, Socket);
end;

procedure TUDPServer.WndProc(var AMsg: TMessage);
var
  Error: word;
begin
  with AMsg do
    case Msg of
      WM_ASYNCSELECT:
        begin
          if (FSocketState = ssClosed) then
            Exit;
          Error:= WSAGetSelectError(LParam);
          case WSAGetSelectEvent(LParam) of
            FD_READ  : IncommingData(WParam, Error);
          else
            if Error <> 0 then
              SocketError(Error);
          end;
        end;
    else
      Result:= DefWindowProc(FHandle, Msg, WParam, LParam);
    end;
end;

procedure TUDPServer.Open;
var
  SockAddrIn: TSockAddrIn;
  SockOpt: LongBool;
begin
  if (FSocketState <> ssClosed) then
    Exit;

  if not GetAnySockAddrIn(FPort, SockAddrIn) then
    Exit;

  FLocalSocket:= socket(PF_INET, FType, 0);
  if FLocalSocket = INVALID_SOCKET then
    begin
      SocketError(WSAGetLastError);
      Exit;
    end;

  if WSAASyncSelect(FLocalSocket, FHandle, WM_ASYNCSELECT, FD_READ) <> 0 then
    begin
      SocketError(WSAGetLastError);
      closesocket(FLocalSocket);
      Exit;
    end;

  SockOpt:= true; {Enable Broadcasting on this Socket}
  if setsockopt(FLocalSocket, SOL_SOCKET, SO_BROADCAST, PChar(@SockOpt), SizeOf(SockOpt)) <> 0 then
    begin
      SocketError(WSAGetLastError);
      closesocket(FLocalSocket);
      Exit;
    end;

  if bind(FLocalSocket, SockAddrIn, SizeOf(SockAddrIn)) <> 0 then
    begin
      SocketError(WSAGetLastError);
      closesocket(FLocalSocket);
      Exit;
    end;

  FSocketState:= ssListening;
end;

procedure TUDPServer.Close;
begin
  if (FSocketState = ssNotStarted) or (FSocketState = ssClosed) then
    Exit;

  SocketClose(FLocalSocket, FHandle);
  if FLocalSocket = INVALID_SOCKET then
    FSocketState:= ssClosed;
end;

procedure TUDPServer.Write(Socket: TSocket; Data: string; var SockAddrIn: TSockAddrIn);
begin
  SocketWriteTo(Socket, 0, Data, SockAddrIn);
end;

function TUDPServer.Read(Socket: TSocket; var SockAddrIn: TSockAddrIn): string;
begin
  Result:= SocketReadFrom(Socket, 0, SockAddrIn);
end;

function TUDPServer.Peek(Socket: TSocket; var SockAddrIn: TSockAddrIn): string;
begin
  Result:= SocketReadFrom(Socket, MSG_PEEK, SockAddrIn);
end;

function TUDPServer.WriteBuffer(Socket: TSocket; Buffer: Pointer; Size: integer; var SockAddrIn: TSockAddrIn): integer;
begin
  Result:= SocketWriteBufferTo(Socket, Buffer, Size, 0, SockAddrIn);
end;

function TUDPServer.ReadBuffer(Socket: TSocket; Buffer: Pointer; Size: integer; var SockAddrIn: TSockAddrIn): integer;
begin
  Result:= SocketReadBufferFrom(Socket, Buffer, Size, 0, SockAddrIn);
end;

end.
