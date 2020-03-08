import pygame


class Parameters():
    """Game parameters stored in one place"""

    def __init__(self):
        self.width = 1600  # Window size
        self.height = 800

        self.start_speed = 8  # Player speed at the beginning
        self.speed_slow = 0.1  # Factor to speed function, which slows player when the mass grow
        self.minimal_speed = 1  # Minimal player speed
        self.moves = {0: (0, 0), 1: (0, 1), 3: (1, -1), 4: (1, 0), 5: (1, 1)}  # Dictionary to convert received signals to displacement

        self.eating_ratio = 0.8  # Percentage of mass gain by eating a player
        self.mass_loss = 0.9  # Percantage of mass which remain after mass loss
        self.mass_loss_interval = 5.0  # Time interval of mass loss
        self.begin_mass = 20  # Starting and lowest player mass

        # Not used in new version
        # self.food_number = 50  # Starting number of foood
        # self.when_new_food = 45  # When to spawn new food
        self.food_spawn_start = 2  # Minimal number of spwan food cells
        self.food_spawn_end = 7  # Macimal number of spawn food cells

        self.food_mass = 1  # Mass which gain the player eating food
        self.food_radius = 7

        self.players = {}   # Empty dictionary of players
        self.food = []  # Empty lis of food cells

        self.max_time = 200000000.  # Maximal game run time

        self.running = True  # State if the player still want to play (client)/the game is running (server)
        self.start_time = 0
        self.game_time = 0
        self.next_loss = 0


param = Parameters()
pygame.font.init()  # Initialize the font, which will be used to show the players nicknames
font = pygame.font.SysFont(None, 40)


class Player():
    """ Class which represent state of out player - key, postion, color, mass(size) and name
    with two functions to draw the player with his name on a pygame window and
    to code our move which later will be send to the server"""

    def __init__(self, playerid, x, y, color, name):
        self.id = playerid  # Allow to find our player in and updated dictionary (key)
        self.x = x
        self.y = y
        self.color = color
        self.mass = param.begin_mass
        self.name = name

    def draw(self, window):
        pygame.draw.circle(window, self.color, (self.x, self.y), self.mass)
        txt = font.render(self.name, 1, (0, 0, 0))
        window.blit(txt, (self.x - txt.get_width()/2, self.y - txt.get_height()/2))

    def move(self):
        keys = pygame.key.get_pressed()  # zwraca slownik kluczy
        mov = 0
        if keys[pygame.K_UP]:   # K_UP is representing the down arrow
            mov -= 1
        if keys[pygame.K_DOWN]:
            mov += 1
        if keys[pygame.K_RIGHT]:
            mov += 4
        if keys[pygame.K_LEFT]:
            mov -= 4
        return mov


class Food():
    """Class which describe the food cell - postion, color and size
    and function to draw the cell on a pygame window"""

    def __init__(self, x, y, color):
        self.x = x
        self.y = y
        self.color = color
        self.radius = 7

    def draw(self, window):
        pygame.draw.circle(window, self.color, (self.x, self.y), self.radius)
