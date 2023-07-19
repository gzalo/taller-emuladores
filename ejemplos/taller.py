import pygame
from pygame.locals import *

pygame.init()

scale = 4
width = 64
height = 32
screen = pygame.Surface((width, height))
window = pygame.display.set_mode((scale*width, scale*height))
pygame.display.set_caption("")


quit = False
while quit == False:
    for event in pygame.event.get():
        if event.type == QUIT:
            quit = True

    keys = pygame.key.get_pressed()

    if keys[K_1]:
        screen.set_at((10, 10), (255, 255, 255))
    else:
        screen.set_at((10, 10), (0, 0, 0))

    window.blit(pygame.transform.scale(screen, (scale*width, scale*height)), (0, 0))

    pygame.display.flip()

pygame.quit()