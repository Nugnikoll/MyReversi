import time
import contextlib

@contextlib.contextmanager
def timespan():
	t = time.time()
	yield
	print("time:", time.time() - t)
