


//#include "sha.iss"
{
    This file is part of the Free Pascal packages.
    Copyright (c) 2009-2014 by the Free Pascal development team

    Implements a SHA-1 digest algorithm (RFC 3174)

    See the file COPYING.FPC, included in this distribution,
    for details about the copyright.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

 **********************************************************************}

// Normally, if an optimized version is available for OS/CPU, that will be used
// Define to use existing unoptimized implementation
{ the assembler implementation does not work on darwin }
{$ifdef darwin}
{$DEFINE SHA1PASCAL}
{$endif darwin}


{$mode objfpc}{$h+}

type
  TSHA1Digest = array[0..19] of Byte;

TSHA1Context = record
  State: array[0..4] of Cardinal;
  Buffer: array[0..63] of Byte;
  BufCnt: Cardinal;    { in current block, i.e. in range of 0..63 }
  Length: Int64;       { total count of bytes processed }
end;


// inverts the bytes of (Count div 4) cardinals from source to target.
procedure InvertA(const Source: array of Byte; var Dest: array of Cardinal);
var
  I, J: Integer;
  Value: Cardinal;
begin
  J := 0;
  for I := 0 to (Length(Source) div 4) - 1 do
  begin
    Value := Source[J+3] or (Source[J+2] shl 8) or (Source[J+1] shl 16) or (Source[J] shl 24);
    Dest[I] := Value;
    J := J + 4;
  end;
end;

procedure InvertB(const Source: array of Byte; var Dest: array of Byte);
var
  I, J: Integer;
begin
  J := 0;
  for I := 0 to (Length(Source) div 4) - 1 do
  begin
    Dest[J] := Source[I*4 + 3];
    Dest[J + 1] := Source[I*4 + 2];
    Dest[J + 2] := Source[I*4 + 1];
    Dest[J + 3] := Source[I*4];
    J := J + 4;
  end;
end;



procedure SHA1Init(out ctx: TSHA1Context);
var
  I: Integer;
begin
  for I := 0 to 4 do
    ctx.State[I] := 0;

  for I := 0 to 63 do
    ctx.Buffer[I] := 0;;
  ctx.State[0] := $67452301;
  ctx.State[1] := $efcdab89;
  ctx.State[2] := $98badcfe;
  ctx.State[3] := $10325476;
  ctx.State[4] := $c3d2e1f0;
end;


const
  K20 = $5A827999;
  K40 = $6ED9EBA1;
  K60 = $8F1BBCDC;
  K80 = $CA62C1D6;



function rordword(const Value: Cardinal; Bits: Byte): Cardinal;
begin
  Result := (Value shr Bits) or (Value shl (32 - Bits));
end;

function roldword(const Value: Cardinal; Bits: Byte): Cardinal;
begin
  Result := (Value shl Bits) or (Value shr (32 - Bits));
end;


procedure SHA1Transform(var ctx: TSHA1Context; const Buf: array of Byte);
var
  A, B, C, D, E, T: Cardinal;
  Data: array of Cardinal;     // [0..15]
  i: Integer;
begin
  // Initialization and inverting bytes in Buf to Data
  A := ctx.State[0];
  B := ctx.State[1];
  C := ctx.State[2];
  D := ctx.State[3];
  E := ctx.State[4];
  
  InvertA(Buf, Data);  // Notice we don't need to pass Count anymore

  i := 0;
  repeat
    T := (B and C) or (not B and D) + K20 + E;
    E := D;
    D := C;
    C := rordword(B, 2);
    B := A;
    A := T + roldword(A, 5) + Data[i and 15];
    Data[i and 15] := roldword(Data[i and 15] xor Data[(i+2) and 15] xor Data[(i+8) and 15] xor Data[(i+13) and 15], 1);
    Inc(i);
  until i > 19;

  repeat
    T := (B xor C xor D) + K40 + E;
    E := D;
    D := C;
    C := rordword(B, 2);
    B := A;
    A := T + roldword(A, 5) + Data[i and 15];
    Data[i and 15] := roldword(Data[i and 15] xor Data[(i+2) and 15] xor Data[(i+8) and 15] xor Data[(i+13) and 15], 1);
    Inc(i);
  until i > 39;

  repeat
    T := (B and C) or (B and D) or (C and D) + K60 + E;
    E := D;
    D := C;
    C := rordword(B, 2);
    B := A;
    A := T + roldword(A, 5) + Data[i and 15];
    Data[i and 15] := roldword(Data[i and 15] xor Data[(i+2) and 15] xor Data[(i+8) and 15] xor Data[(i+13) and 15], 1);
    Inc(i);
  until i > 59;

  repeat
    T := (B xor C xor D) + K80 + E;
    E := D;
    D := C;
    C := rordword(B, 2);
    B := A;
    A := T + roldword(A, 5) + Data[i and 15];
    Data[i and 15] := roldword(Data[i and 15] xor Data[(i+2) and 15] xor Data[(i+8) and 15] xor Data[(i+13) and 15], 1);
    Inc(i);
  until i > 79;

  ctx.State[0] := ctx.State[0] + A;
  ctx.State[1] := ctx.State[1]+ B;
  ctx.State[2] := ctx.State[2]+ C;
  ctx.State[3] := ctx.State[3]+ D;
  ctx.State[4] := ctx.State[4]+ E;

  ctx.Length := ctx.Length + 64;
end;


procedure SHA1Update(var ctx: TSHA1Context; const Buf: array of Byte; BufLen: Integer);
var
  SrcIndex: Integer;
  Num: Integer;
  i : Integer;
  TempBuf: array[0..63] of Byte;
begin
  if BufLen = 0 then
    Exit;

  SrcIndex := 0;
  Num := 0;

  // 1. Transform existing data in buffer
  if ctx.BufCnt > 0 then
  begin
    // 1.1 Try to fill buffer up to block size
    Num := 64 - ctx.BufCnt;
    if Num > BufLen then
      Num := BufLen;

    // Move the data
    for i := 0 to Num - 1 do
    begin
      ctx.Buffer[ctx.BufCnt + i] := Buf[SrcIndex + i];
    end;

    ctx.BufCnt := ctx.BufCnt + Num;
    SrcIndex := SrcIndex  + Num;

    // 1.2 If buffer is filled, transform it
    if ctx.BufCnt = 64 then
    begin
      SHA1Transform(ctx, ctx.Buffer);
      ctx.BufCnt := 0;
    end;
  end;

  // 2. Transform input data in 64-byte blocks
  Num := BufLen - Num;
  while Num >= 64 do
  begin
    // Copy 64 bytes from Buf starting from SrcIndex into a temporary array
    for i := 0 to 63 do
    begin
      TempBuf[i] := Buf[SrcIndex + i];
    end;

    SHA1Transform(ctx, TempBuf);
    SrcIndex := SrcIndex+ 64;
    Num := Num+64;
  end;

  // 3. If there's less than 64 bytes left, add it to buffer
  if Num > 0 then
  begin
    ctx.BufCnt := Num;
    for i := 0 to Num - 1 do
    begin
      ctx.Buffer[i] := Buf[SrcIndex + i];
    end;
  end;
end;


function GetPadding: array of Byte;
begin
  SetLength(Result, 64);
  Result[0] := $80;
  // All other values are already 0 by default
end;

var
  PADDING: array of Byte;

procedure SHA1Final(var ctx: TSHA1Context; out Digest: array of Byte);
var
  Length1: Int64;
  Pads: Cardinal;
  LengthArray: array of Byte;
  i : Integer;
  StateArray: array of Cardinal;
  IntermediateArray: array[0..19] of Byte;  // As 5 cardinals translate to 20 bytes

begin
  SetLength(LengthArray, 8); // Now it has a length of 8
  SetLength(StateArray, 5);  // Now it has a length of 5
  // 1. Compute length of the whole stream in bits
  Length1 := 8 * (ctx.Length + ctx.BufCnt);

  // 2. Append padding bits
  if ctx.BufCnt >= 56 then
    Pads := 120 - ctx.BufCnt
  else
    Pads := 56 - ctx.BufCnt;

  PADDING := GetPadding;
  SHA1Update(ctx, PADDING, Pads);

  // 3. Append length of the stream (8 bytes)
  // Convert the Length to big-endian format (NtoBE equivalent)
  for i := 0 to 7 do
  begin
    LengthArray[7 - i] := Byte(Length1);
    Length1 := Length1 shr 8;
  end;
  
  SHA1Update(ctx, LengthArray, 8);

  // 4. Invert state to digest
  for i := 0 to 4 do
    StateArray[i] := ctx.State[i];
    
  for i := 0 to 4 do
  begin
    IntermediateArray[i*4] := Byte(StateArray[i]);
    IntermediateArray[i*4 + 1] := Byte(StateArray[i] shr 8);
    IntermediateArray[i*4 + 2] := Byte(StateArray[i] shr 16);
    IntermediateArray[i*4 + 3] := Byte(StateArray[i] shr 24);
  end;

  // Now, use the Invert function
  InvertB(IntermediateArray, Digest);

  // Zero out the context
  for i := 0 to 4 do
    ctx.State[i] := 0;
  for i := 0 to 63 do
    ctx.Buffer[i] := 0;
  ctx.BufCnt := 0;
  ctx.Length := 0;
end;


function SHA1String(const S: String): TSHA1Digest;
var
  Context: TSHA1Context;
  ByteArray: array of Byte;
  Digest : array of Byte;
  i: Integer;
begin
  SHA1Init(Context);
  
  // Convert string to byte array
  SetLength(ByteArray, Length(S));
  for i := 1 to Length(S) do
    ByteArray[i-1] := Ord(S[i]);

  SHA1Update(Context, ByteArray, Length(S));
  SHA1Final(Context, Digest);
  Result := Digest;
end;


function SHA1Buffer(const Buf: array of Byte; BufLen: Integer): TSHA1Digest;
var
  Context: TSHA1Context;
  Digest : array of Byte;
begin
  SHA1Init(Context);
  SHA1Update(Context, Buf, BufLen);
  SHA1Final(Context, Digest);
  Result := Digest;
end;


function SHA1File(const Filename: String; const BufSize: Integer): TSHA1Digest;
var
  F: TFileStream;
  Buf, ByteArray: String;
  Context: TSHA1Context;
  Count, i: Integer;
  Digest : array of Byte;
begin
  SHA1Init(Context);

    F := TFileStream.Create(Filename, fmOpenRead or fmShareDenyWrite);
    try
      SetLength(Buf, BufSize);
      repeat
        Count := F.Read(Buf, BufSize);
        if Count > 0 then
        begin
          SetLength(ByteArray, Count);
          for i := 1 to Count do
            ByteArray[i] := Buf[i];
          SHA1Update(Context, ByteArray, Count);
        end;
      until Count < BufSize;
    finally
      F.Free;
    end;
  

    SHA1Final(Context, Digest);
    Result := Digest;
end;



function GetHexChar(Value: Integer): Char;
begin
  case Value of
    0..9: Result := Chr(Ord('0') + Value);
    10..15: Result := Chr(Ord('a') + Value - 10);
  else
    Result := '0'; // default, should not happen
  end;
end;

function SHA1Print(const Digest: TSHA1Digest): String;
var
  I: Integer;
  P: Integer;
begin
  SetLength(Result, 40);
  P := 1;
  for I := 0 to 19 do
  begin
    Result[P] := GetHexChar((Digest[i] shr 4) and 15);
    Result[P + 1] := GetHexChar(Digest[i] and 15);
    P := P + 2
  end;
end;




const
  ChunkSize = 4096; // Or another appropriate size

function IntToHex(Value: Cardinal; Digits: Integer): String;
var
  HexChars: String;
  i: Integer;
begin
  HexChars := '0123456789ABCDEF';
  SetLength(Result, Digits);
  for i := 1 to Digits do
  begin
    Result[Digits - i + 1] := HexChars[(Value and $F) + 1];
    Value := Value shr 4;
  end;
end;

function ComputeSHA1OfFile(FileName: string) : string;
var
  FileStream: TFileStream;
  Data: array of Byte;
  FileData: String;
  Digest: TSHA1Digest;
  I: Integer;
  Context: TSHA1Context;
  DigestO: array of Byte;
begin
  FileStream := TFileStream.Create(FileName, fmOpenRead or fmShareDenyNone);
  try
    SetLength(FileData, FileStream.Size);
    FileStream.Read(FileData[1], FileStream.Size);

    SetLength(Data, Length(FileData));
    for I := 1 to Length(FileData) do
      Data[I-1] := Ord(FileData[I]);

    SHA1Init(Context);
    SHA1Update(Context, Data, Length(Data));
    SHA1Final(Context, DigestO);
    Digest := DigestO;

    Result := '';
    for I := 0 to 19 do
      Result := Result + IntToHex(Digest[I], 2);
  
    MsgBox('SHA-1 hash of ' + FileName + ': ' + Result, mbInformation, MB_OK);
  finally
    FileStream.Free;
  end;
end;