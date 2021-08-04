import sys
import json
import bson
import numpy as np

sys.path.append("../python")
import reversi as rv

def load(fobj):
	data = fobj.read(4)
	if data != b"data":
		return

	mode = fobj.read(4)
	if not mode in (b"bson", b"json"):
		return

	l = fobj.read(4)
	l = int.from_bytes(l, "little")
	if mode == b"bson":
		fobj.seek(-4, 1)
		data = fobj.read(l)
		data = bson.loads(data)
	elif mode == b"json":
		data = fobj.read(l)
		data = data.decode("utf-8")
		data = json.loads(data)
	return data

def dump(fobj, data, mode = "bson"):
	if not mode in ("bson", "json"):
		return
	fobj.write(b"data")
	if mode == "bson":
		fobj.write(b"bson")
		fobj.write(bson.dumps(data))
	elif mode == "json":
		fobj.write(b"json")
		s = json.dumps(data, separators=(",", ":"))
		l = len(s)
		fobj.write(l.to_bytes(4, "little"))
		fobj.write(s.encode("utf-8"))

def load_list(fobj, name_list = None, need_full = False):
	if name_list is None:
		name_list = {}
	data_list = []
	data_map = {}

	def get_result():
		select_list = list(map(data_map.get, name_list))
		if need_full:
			return head, select_list, data_list
		else:
			return select_list

	head = load(fobj)
	if head is None or not "append" in head:
		return get_result()

	data_map = dict(zip(name_list, (None,) * len(name_list)))

	for item in head["append"]:
		if not "name" in item or not "type" in item:
			raise RuntimeError("corrupted file information")
		name = item["name"]
		item_type = item["type"]
		if item_type == "npy":
			data = np.load(fobj)
		elif item_type == "pattern":
			buf = fobj.read(rv.pattern.csize * 4)
			arr = np.frombuffer(buf, dtype = np.float32)
			data = rv.pattern()
			data.from_compact(arr)
		else:
			raise RuntimeError("unrecognized data type")
		data_list.append(data)
		if not name in data_map:
			continue
		data_map[name] = data

	return get_result()

def dump_list(fobj, head, data_list, mode = "bson"):
	dump(fobj, head, mode = mode)
	for i in data_list:
		if type(i) is np.array:
			np.save(fobj, i)
		elif type(i) is rv.pattern:
			fobj.write(i.compact().tobytes())
		else:
			raise RuntimeError("unrecognized data type")
