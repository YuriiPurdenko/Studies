import socket
import pickle
import time
import random
import math
from _thread import start_new_thread
from player import Player, Food, Parameters

param = Parameters()


def speedof(players, key):
    """Function calculating speed of given player"""

    return max(2, param.start_speed - players[key].mass * param.speed_slow)


def getmoves(players, key, var):
    """Convert received code into move direction in (x,y) axis
    and move the player in given direction"""

    if var < 0:
        x, y = (-param.moves[-var][0], -param.moves[-var][1])
    else:
        x, y = param.moves[var]

    players[key].x += x * speedof(players, key)
    players[key].y += y * speedof(players, key)

    if players[key].x - players[key].mass < 0:
            players[key].x = players[key].mass

    if players[key].y - players[key].mass < 0:
            players[key].y = players[key].mass

    if players[key].x + players[key].mass > param.width:
            players[key].x = param.width - players[key].mass

    if players[key].y + players[key].mass > param.height:
            players[key].y = param.height - players[key].mass

    players[key].x = round(players[key].x)
    players[key].y = round(players[key].y)


def massloss(players, key):
    """Function which reduce constant percentage mass of the player
    is called in time intervals"""

    # Player mass can not drop bellow beginning mass
    players[key].mass = max(math.floor(players[key].mass * param.mass_loss), param.begin_mass)


def dist(x1, y1, x2, y2):
    """Calculate distance between two points on two-dimensional space"""

    return math.sqrt((x1-x2)*(x1-x2) + (y1-y2)*(y1-y2))


def p1_eat_p2(players, p1, p2):
    """Simulates eating player with key p2 by player with k2
    and spawns player with key p2 in a new place"""

    players[p2].x = -1000  # Lower risk of concurrency problem - other player still could be eaten by me
    players[p1].mass += math.floor(players[p2].mass * param.eating_ratio)  # Eater gain a certain percantage of the eaten player
    players[p2].mass = param.begin_mass
    spawn(players, p2)
    print("[Game]" + players[p1].name + " ate " + players[p2].name)


def collision(players, key):
    """Check if there exist a player try to eat me
    and then eats me if he is bigger then me"""

    for p1 in players:
        d = dist(players[p1].x, players[p1].y, players[key].x, players[key].y)
        if d < players[p1].mass + players[key].mass:
            if players[key].mass < players[p1].mass:
                p1_eat_p2(players, p1, key)


def eating(players, key, food):
    """Check if there exist a player eating a food cell
    and simulate it"""

    for f in food:
        d = dist(players[key].x, players[key].y, f.x, f.y)
        if d < param.food_radius + players[key].mass:
            players[key].mass += param.food_mass
            food.remove(f)
            newfood(players, food)


def newfood(players, food):
    """Randomly generate a new food cell (in a free spot)"""

    while True:
        run = True
        x = random.randrange(0, param.width)
        y = random.randrange(0, param.height)
        for p in players:
            d = dist(players[p].x, players[p].y, x, y)
            if d < param.food_radius + players[p].mass:
                run = False
        if run:
            break
    food.append(Food(x, y, color()))


def addfood(players, food):
    """Function adding random amount of food
    from range .food_spawn_start, food_spawn_end"""

    for i in range(random.randrange(param.food_spawn_start, param.food_spawn_end)):
                    newfood(players, food)


def color():
    """Randomly generate a color (which is not too bright)"""

    r = g = b = 255
    while (r + g + b) > (255 * 3 - 20) or (r + g + b) < 20:
        r = random.randrange(0, 255)
        g = random.randrange(0, 255)
        b = random.randrange(0, 255)
    return (r, g, b)


def spawn(players, key):
    """Randomly generate a new spot for player with given key
     which do not collide with other players"""

    while True:
        run = True
        x = random.randrange(0, param.width)
        y = random.randrange(0, param.height)
        for p in players:
            d = dist(players[p].x, players[p].y, x, y)
            if d < param.begin_mass + players[p].mass:
                run = False
        if run:
            break
    players[key].x = x
    players[key].y = y


def receive(conn):
    """Function receiving data from client"""

    data = conn.recv(10000)
    if not data:
        return None
    return pickle.loads(data)


def send(conn, data):
    """Function sending data to the client"""
    conn.send(pickle.dumps(data))


def thread_client(conn, playerid):
    """Function working for each client in a diffrent thread
    which handle the client, receive data from him,
    send information about the game and process game logic"""

    global param
    name = receive(conn)  # receive name of our player from client
    if name is None:
        print("Problem receiving player name")
        conn.close()
        return
    p = Player(playerid, 0, 0, color(), name)  # initialize new player
    param.players[playerid] = p  # add player to our dictionary
    spawn(param.players, playerid)  # spawn him on the map
    send(conn, param.players[playerid])
    while True:
        if param.running:
            param.game_time = time.time() - param.start_time
            if param.game_time > param.max_time:
                param.running = False
        try:
            data = receive(conn)
            if data is None:    # double receive to avoid problem with [Errno 32] or [Errno 104] - 123 inform us that client want to close
                print("Problem receiving player move")
                break
            if data == 123:
                break
            getmoves(param.players, playerid, data)

            if param.running:
                collision(param.players, playerid)
                eating(param.players, playerid, param.food)
                if (param.game_time/param.mass_loss_interval) > param.next_loss:
                    param.next_loss += 1
                    massloss(param.players, playerid)

            # if len(param.food) < param.when_new_food: in this version we have a constant amount of food on board
            #    addfood(param.players, param.food)
            send(conn, param.players)
            data = receive(conn)
            send(conn, param.food)
            if data is None:
                print("Problem receiving player move")
                break

        except Exception as e:
            print(e)
            break
    # time.sleep(0.001)
    del param.players[playerid]
    print("Client " + str(playerid) + " disconnected")
    conn.close()  # Important to close open file descriptors


def communication_init():
    """Function which initiate the server(communication)"""

    server = socket.gethostbyname(socket.gethostname() + ".local")  # Geting server ip
    port = 10000  # Setting port used to communication with clients
    print(server)
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)  # Communication using IPv4 with streams
    s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)  # Setting options to socket
    try:
        s.bind((server, port))
    except socket.error as e:
        print(str(e))
        print("Problem starting server")
        quit()
    s.listen()
    print("Server Started")
    return s


def main(s):
    """Main loop of the program which is waiting for clients
    trying to connect and if a connection is established
    start a new thread which will handle the client"""

    addfood(param.players, param.food)
    running = False
    playerid = 0
    while True:
        conn, addr = s.accept()
        print("[CONNECTION] Client: " + str(playerid) + " connected to:", addr)

        if not(running):
            running = True
            param.start_time = time.time()
            print("Game started")

        start_new_thread(thread_client, (conn, playerid))
        playerid += 1
    print("Server finnished work")


if __name__ == "__main__":
    s = communication_init()
    main(s)
