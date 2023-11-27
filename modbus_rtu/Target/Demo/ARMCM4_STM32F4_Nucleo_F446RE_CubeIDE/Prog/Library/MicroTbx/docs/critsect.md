# Critical sections

In an interrupt driven embedded software program, problems can occur with global resource sharing. A global resource can for example be a global data structure, a peripheral, or a communication device.

Imagine a situation where the software program is writing to a global variable and before the write operation is completed, an interrupt occurred. The associated interrupt handler could write to the same global variable. After the interrupt is completed, to initial write operation completes. The global variable now holds invalid data.

Such data concurrency issues are resolved by performing the global resource access in a critical section. In a critical section, no interrupt can occur and you can therefore be certain that you have mutual exclusive access to the global resource.

## Usage

In MicroTBX a critical section is entered by calling the function [`TbxCriticalSectionEnter()`](apiref.md#tbxcriticalsectionenter). After this the global resource can be safely accessed. Once done, the critical section is left by calling the function [`TbxCriticalSectionExit()`](apiref.md#tbxcriticalsectionexit).

This means that both functions must always be used pair-wise. Each call to [`TbxCriticalSectionEnter()`](apiref.md#tbxcriticalsectionenter) must eventually be followed by a call to [`TbxCriticalSectionExit()`](apiref.md#tbxcriticalsectionexit). Note that MicroTBX supports nested critical sections. It is completely fine if your software program enters the critical section again, even if it is already in a critical section.

## Examples

The following example contains a global variable `myMessage` and a function `TransmitMessage()`. Imagine that this function can be called both from the main program loop and from an interrupt. This makes variable `myMessage` a shared resource. A critical section is applied to obtain mutual exclusive access to the variable `myMessage`:

```c
struct
{
  uint32_t id;
  uint8_t  len;
  uint8_t  data[8];
} myMessage;

void TransmitMessage(void)
{
  /* Obtain mutual exclusive access to myMessage. */
  TbxCriticalSectionEnter();
  /* Prepare and send the message. */
  myMessage.id++;
  myMessage.len = 1u;
  myMessage.data[0]++;
  SendMessage(&myMessage);
  /* Release mutual exclusive access to myMessage. */
  TbxCriticalSectionExit();
}
```
