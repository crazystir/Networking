from __future__ import print_function
import sys

# Print to the stderr
def errprint(*args, **kwargs):
    print(*args, file=sys.stderr, **kwargs)