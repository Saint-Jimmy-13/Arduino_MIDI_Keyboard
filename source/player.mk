CC = gcc
CFLAGS = -Wall -O2
LDFLAGS = -lasound

midi_player:	midi_player.c
				$(CC) $(CFLAGS) -o midi_player midi_player.c $(LDFLAGS)

clean:
		rm -f midi_player
