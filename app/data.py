import json
import bson
import numpy as np

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
		data = json.loads(fobj)
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
		s = json.dumps(data)
		l = len(s)
		fobj.write(l.to_bytes(4, "little"))
		fobj.write(s)

def load_npy_list(fobj, name_list = None, need_full = False):
	if name_list is None:
		name_list = {}
	npy_list = []
	npy_map = {}

	def get_result():
		select_list = list(map(npy_map.get, name_list))
		if need_full:
			return head, select_list, npy_list
		else:
			return select_list

	head = load(fobj)
	if head is None or not "append" in head:
		return get_result()

	npy_map = dict(zip(name_list, (None,) * len(name_list)))

	for item in head["append"]:
		if not "name" in item or not "type" in item or item["type"] != "npy":
			break
		name = item["name"]
		data = np.load(fobj)
		npy_list.append(data)
		if not name in npy_map:
			continue
		npy_map[name] = data

	return get_result()

def dump_npy_list(fobj, head, npy_list,):
	dump(fobj, head)
	for i in npy_list:
		np.save(fobj, i)
