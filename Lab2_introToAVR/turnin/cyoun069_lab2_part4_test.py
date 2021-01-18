# Array of tests to run (in order)
# Each test contains
#   description - 
#   steps - A list of steps to perform, each step can have
#       inputs - A list of tuples for the inputs to apply at that step
#       *time - The time (in ms) to wait before continuing to the next step 
#           and before checking expected values for this step. The time should be a multiple of
#           the period of the system
#       *iterations - The number of clock ticks to wait (periods)
#       expected - The expected value at the end of this step (after the "time" has elapsed.) 
#           If this value is incorrect the test will fail early before completing.
#       * only one of these should be used
#   expected - The expected output (as a list of tuples) at the end of this test
# An example set of tests is shown below. It is important to note that these tests are not "unit tests" in 
# that they are not ran in isolation but in the order shown and the state of the device is not reset or 
# altered in between executions (unless preconditions are used).
tests = [ 
	{'description': '00001111, 00001111, 00001111 = PORTD: 0x08 (00001000)', 
	'steps': [ {'inputs': [('PINA',0x0F), ('PINB',0x0F), ('PINC',0x0F)], 'iterations': 5 } ],
	'expected': [('PORTD',0x08)],
	},

	{'description': '00100010, 00101000, 00100010 = PORTD: 0x18 (00011000)', 
	'steps': [ {'inputs': [('PINA',0x22), ('PINB',0x28), ('PINC',0x22)], 'iterations': 5 } ],
	'expected': [('PORTD',0x18)],
	},
	
	{'description': '01100000, 01001100, 00101111 = PORTD: 0x35 (00110101)', 
	'steps': [ {'inputs': [('PINA',0x60), ('PINB',0x4C), ('PINC',0x2F)], 'iterations': 5 } ],
	'expected': [('PORTD',0x35)],
	},

	{'description': '01100000, 00000000, 00000001 = PORTD: 0x1A (00011010)', 
	'steps': [ {'inputs': [('PINA',0x60), ('PINB',0x00), ('PINC',0x01)], 'iterations': 5 } ],
	'expected': [('PORTD',0x1A)],
	},

	{'description': '00000001, 00000000, 01100000 = PORTD: 0x1A (00011010)', 
	'steps': [ {'inputs': [('PINA',0x60), ('PINB',0x00), ('PINC',0x01)], 'iterations': 5 } ],
	'expected': [('PORTD',0x1A)],
	},
    ]

# Optionally you can add a set of "watch" variables these need to be global or static and may need
# to be scoped at the function level (for static variables) if there are naming conflicts. The 
# variables listed here will display everytime you hit (and stop at) a breakpoint
#watch = ['PORTB']

