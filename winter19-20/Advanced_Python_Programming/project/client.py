import pygame
from network import Network
from player import Parameters


param = Parameters()


def getname():
    """ Return name of the player from terminal"""

    while True:
        name = input("Your name: ")
        if 0 < len(name) <= 6:
            break
        else:
            print("Name must be a string in range from 1 to 6")
    return name


def redrawWindow(window, players, food):
    """Updates the window with current state of the object"""

    window.fill((255, 255, 255))
    for x in players:
        players[x].draw(window)
    for f in food:
        f.draw(window)
    pygame.display.update()


def init(width, height, name):
    """Initalize the window, pygame clock and
    the communication with the server (send player name)"""

    win = pygame.display.set_mode((width, height))
    pygame.display.set_caption("Agar.wp")
    server = Network()
    player = server.connect(name)
    clock = pygame.time.Clock()
    return (win, server, player,     clock)


def main(name):
    """Main loop which handle pygame events,
     communication with the server(send our move)
      and calls function to update our window"""

    global param
    win, server, player, clock = init(param.width, param.height, name)
    while param.running:
        clock.tick(60)  # The game won't run more then 60 frames per second
        moves = player.move()
        param.players = server.send(moves)
        param.food = server.send(moves)
        player = param.players[player.id]

        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                param.running = False

            if event.type == pygame.KEYDOWN:
                if event.key == pygame.K_ESCAPE:
                    param.running = False

        redrawWindow(win, param.players, param.food)

    server.diconnect()
    pygame.quit()
    quit()


if __name__ == "__main__":
    name = getname()
    main(name)
