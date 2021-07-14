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

def load_npy_list(fobj, name_list):
	name_map = dict(zip(name_list, (None,) * len(name_list)))

	def get_result():
		result = []
		for name in name_list:
			result.append(name_map[name])
		return result

	head = load(fobj)
	if head is None or not "append" in head:
		return get_result()

	for item in head["append"]:
		if not "name" in item or not "type" in item or item["type"] != "npy":
			break
		name = item["name"]
		data = np.load(fobj)
		if not name in name_map:
			continue
		name_map[name] = data

	return get_result()

def dump_npy_list(fobj, name_list, *np_list, modify_head = None):
	head = []
	for name in name_list:
		head.append({"name": name, "type": "npy"})
	head = {"append": head}
	if modify_head:
		modify_head(head)
	dump(fobj, head)
	for i in np_list:
		np.save(fobj, i)
