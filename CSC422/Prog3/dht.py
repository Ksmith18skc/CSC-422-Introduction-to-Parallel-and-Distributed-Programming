import sys
from mpi4py import MPI
from time import sleep
from dht_globals import *  # defines ADD, GET, PUT, REMOVE, ACK, RETVAL, END, MAX
from command import commandNode  # main loop for command node

def headEnd():
    dummy = None
    MPI.COMM_WORLD.recv(source=numProcesses - 1, tag=END)
    for i in range(1, numProcesses - 1):
        MPI.COMM_WORLD.send(dummy, dest=i, tag=END)
    MPI.Finalize()
    sys.exit(0)

def storageEnd():
    dummy = MPI.COMM_WORLD.recv(source=0, tag=END)
    MPI.Finalize()
    sys.exit(0)

def getKeyVal(source):
    key = MPI.COMM_WORLD.recv(source=source, tag=GET)
    if key in storage:
        value = storage[key]
        result = (value, myStorageId)
        MPI.COMM_WORLD.send(result, dest=0, tag=RETVAL)
    else:
        if childRank is not None:
            MPI.COMM_WORLD.send(key, dest=childRank, tag=GET)

def handleMessages():
    status = MPI.Status()
    while True:
        MPI.COMM_WORLD.probe(source=MPI.ANY_SOURCE, tag=MPI.ANY_TAG, status=status)
        source = status.Get_source()
        tag = status.Get_tag()

        if tag == END:
            if myRank == 0:
                headEnd()
            else:
                storageEnd()

        elif tag == ADD:
            if myRank == 0:
                rank, new_id = MPI.COMM_WORLD.recv(source=numProcesses - 1, tag=ADD)
                active_nodes[rank] = new_id
                updateRing()
                MPI.COMM_WORLD.send((rank, new_id), dest=rank, tag=ADD)
                MPI.COMM_WORLD.send(0, dest=numProcesses - 1, tag=ACK)
            else:
                global myStorageId
                rank, new_id = MPI.COMM_WORLD.recv(source=0, tag=ADD)
                if myRank == rank:
                    myStorageId = new_id
                    updateRing()

        elif tag == PUT:
            if myRank == 0:
                key, value = MPI.COMM_WORLD.recv(source=numProcesses - 1, tag=PUT)
                target_rank = routeKeyToRank(key)
                if not isinstance(target_rank, int):
                    raise TypeError(f"Invalid target_rank: {target_rank}")
                MPI.COMM_WORLD.send((key, value), dest=target_rank, tag=PUT)
                MPI.COMM_WORLD.send(0, dest=numProcesses - 1, tag=ACK)
            else:
                key, value = MPI.COMM_WORLD.recv(source=0, tag=PUT)
                storage[key] = value

        elif tag == REMOVE:
            if myRank == 0:
                id_to_remove = MPI.COMM_WORLD.recv(source=numProcesses - 1, tag=REMOVE)
                target_rank = None
                for rank, sid in active_nodes.items():
                    if sid == id_to_remove:
                        target_rank = rank
                        break
                if target_rank is not None:
                # Tell node to redistribute its data first
                    MPI.COMM_WORLD.send(id_to_remove, dest=target_rank, tag=REMOVE)

                # Wait until redistribution is complete
                MPI.COMM_WORLD.recv(source=target_rank, tag=ACK)

                # Now it is safe to modify the ring
                del active_nodes[target_rank]
                updateRing()

                MPI.COMM_WORLD.send(0, dest=numProcesses - 1, tag=ACK)
            else:
                id_to_remove = MPI.COMM_WORLD.recv(source=0, tag=REMOVE)
                if myStorageId == id_to_remove:
                    for key, value in list(storage.items()):
                        new_rank = routeKeyToRank(key)
                        if new_rank != myRank:
                            MPI.COMM_WORLD.send((key, value), dest=new_rank, tag=PUT)
                            del storage[key]
                    updateRing()
                    myStorageId = None
                    MPI.COMM_WORLD.send(0, dest=0, tag=ACK)

        elif tag == GET:
            if myRank == 0:
                key = MPI.COMM_WORLD.recv(source=numProcesses - 1, tag=GET)
                target_rank = routeKeyToRank(key)
                MPI.COMM_WORLD.send(key, dest=target_rank, tag=GET)
            else:
                getKeyVal(source)

        elif tag == ACK:
            pass

        elif tag == RETVAL:
            if myRank == 0:
                retval = MPI.COMM_WORLD.recv(source=source, tag=RETVAL)
                MPI.COMM_WORLD.send(retval, dest=numProcesses - 1, tag=RETVAL)
            else:
                value, storage_id = MPI.COMM_WORLD.recv(source=source, tag=RETVAL)
                print(f"val is {value}, storage id is {storage_id}")

def routeKeyToRank(key):
    sorted_nodes = sorted(active_nodes.items(), key=lambda x: x[1])  # (rank, sid)
    for i in range(len(sorted_nodes)):
        rank, sid = sorted_nodes[i]
        parent_sid = sorted_nodes[i - 1][1] if i > 0 else 0
        if parent_sid < key <= sid:
            return rank
    if permanent_node_rank is not None:
        return permanent_node_rank
    raise ValueError(f"Unable to route key {key} to a valid rank. Current active_nodes: {active_nodes}")

def updateRing():
    global childRank
    sorted_nodes = sorted(active_nodes.items(), key=lambda x: x[1])  # (rank, sid)
    for i in range(len(sorted_nodes)):
        rank, sid = sorted_nodes[i]
        next_rank = sorted_nodes[(i + 1) % len(sorted_nodes)][0]
        if myRank == rank:
            childRank = next_rank

# === MPI Global Setup ===
myStorageId = None
childRank = None
active_nodes = {}  # key: rank, value: assigned storage id
permanent_node_rank = None

if __name__ == "__main__":
    numProcesses = MPI.COMM_WORLD.Get_size()
    myRank = MPI.COMM_WORLD.Get_rank()
    storage = {}

    if myRank == 0:
        myStorageId = 0
        permanent_node_rank = numProcesses - 2

    elif myRank == numProcesses - 2:
        myStorageId = MAX
        permanent_node_rank = myRank
        active_nodes[myRank] = MAX
    else:
        myStorageId = None

    if myRank < numProcesses - 1:
        handleMessages()
    else:
        commandNode()
