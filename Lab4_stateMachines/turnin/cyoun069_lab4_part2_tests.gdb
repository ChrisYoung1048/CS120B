# Test file for "cyoun069_lab4_part2.c"


# commands.gdb provides the following functions for ease:
#   test "<message>"
#       Where <message> is the message to print. Must call this at the beginning of every test
#       Example: test "PINA: 0x00 => expect PORTC: 0x01"
#   checkResult
#       Verify if the test passed or failed. Prints "passed." or "failed." accordingly, 
#       Must call this at the end of every test.
#   expectPORTx <val>
#       With x as the port (A,B,C,D)
#       The value the port is epected to have. If not it will print the erroneous actual value
#   setPINx <val>
#       With x as the port or pin (A,B,C,D)
#       The value to set the pin to (can be decimal or hexidecimal
#       Example: setPINA 0x01
#   printPORTx f OR printPINx f 
#       With x as the port or pin (A,B,C,D)
#       With f as a format option which can be: [d] decimal, [x] hexadecmial (default), [t] binary 
#       Example: printPORTC d
#   printDDRx
#       With x as the DDR (A,B,C,D)
#       Example: printDDRB

echo ======================================================\n
echo Running all tests..."\n\n

test "PINA: 0x00 -> PORTC: 0x07"
set State = Init
setPINA 0x00
continue 2
expectPORTC 0x07
checkResult

test "PINA: 0x00, 0x01, 0x00, 0x01 -> PORTC: 0x09"
set State = Init
setPINA 0x00
continue 2
setPINA 0x01
continue 2
setPINA 0x00
continue 2
setPINA 0x01
continue 2
expectPORTC 0x09
checkResult

test "PINC: 0x09, PINA: 0x00, 0x01 -> PORTC: 0x09"
set State = Init
setPINC 0x09
setPINA 0x00
continue 2
setPINA 0x01
continue 2
expectPORTC 0x09
checkResult

test "PINA: 0x00, 0x02, 0x00, 0x02 -> PORTC: 0x05"
set State = Init
setPINA 0x00
continue 2
setPINA 0x02
continue 2
setPINA 0x00
continue 2
setPINA 0x02
continue 2
expectPORTC 0x05
checkResult

test "PINC: 0x00, PINA: 0x00, 0x01 -> PORTC: 0x00"
set State = Init
setPINC 0x00
setPINA 0x00
continue 2
setPINA 0x02
continue 2
expectPORTC 0x00
checkResult

test "PINA: 0x00, 0x01, 0x03 -> PORTC: 0x00"
set State = Init
setPINA 0x00
continue 2
setPINA 0x01
continue 2
setPINA 0x03
continue 2
expectPORTC 0x00
checkResult

test "PINA: 0x00, 0x02, 0x03 -> PORTC: 0x00"
set State = Init
setPINA 0x00
continue 2
setPINA 0x02
continue 2
setPINA 0x03
continue 2
expectPORTC 0x00
checkResult

# Report on how many tests passed/tests ran
set $passed=$tests-$failed
eval "shell echo Passed %d/%d tests.\n",$passed,$tests
echo ======================================================\n
