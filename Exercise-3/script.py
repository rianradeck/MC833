import os
import time
import signal
import subprocess
from pprint import pprint
import re
import matplotlib.pyplot as plt
import matplotlib
from subprocess import Popen
import numpy as np

CLIENTS = 50
BACKLOG = 40

procs = [ Popen("./cliente 127.0.0.1 1234 > \\dev\\null", shell=True) for _ in range(CLIENTS) ]

estab = 0
syn = 0
estab_ot = [0]
syn_ot = [CLIENTS]
time_ot = [0]
at_t = 0.01
while estab != CLIENTS:
	# pass:
	# print(cnt)
	# cnt -=

	process = []
	x = str(subprocess.check_output("netstat -taulpn".split()))
	x = x.split("\\n")
	for i in range(len(x)):
		x[i] = str(x[i])
		re.sub(' +', ' ', x[i])
		# print(x[i].split())
		if len(x[i].split()) > 4 and "1234" in x[i].split()[4]:
			process.append(x[i])

	estab = 0
	syn = 0
	for p in process:
		# print("PROCESS:", p)
		if "ESTABLISHED" in p:
			estab += 1
		if "SYN_SENT" in p:
			syn += 1
	print(f"ESTABLISHED: {estab}, SYN_SENT: {syn}")

	estab_ot.append(estab)
	syn_ot.append(syn)
	time_ot.append(at_t)
	time.sleep(0.01)
	at_t += 0.01

font = {'family': 'normal', 'size': 22}
matplotlib.rc('font', **font)

plt.plot(time_ot, estab_ot, label="ESTABLISHED")
plt.plot(time_ot, syn_ot, label = "SYN_SENT")
plt.legend()
plt.xticks(np.arange(0, max(time_ot), 0.1))
plt.yticks(np.arange(0, CLIENTS + 1, 5))
plt.grid(alpha=0.3)
plt.title(f"Backlog: {BACKLOG}, Clientes simultâneos: {CLIENTS}")
plt.ylabel("Quantidade de conexões")
plt.xlabel("Tempo (s)")
plt.show()

input()
# pprint(x)

# time.sleep(30)
for p in procs:
   p.kill()
exit()

# for i in range(n):
# 	os.killpg(os.getpgid(processes[i].pid), signal.SIGTERM)
	