# Philosophers

A C implementation of the classic **Dining Philosophers** problem — the 42 school project on threads, mutexes, processes and semaphores.

Philosophers sit around a round table. Between each pair of philosophers lies a single fork. To eat, a philosopher must hold the fork on their left and the fork on their right. When they are not eating, they are either sleeping or thinking. If a philosopher does not eat for too long, they die.

The project is split in two parts:

- **`philo/`** — mandatory. One process, one thread per philosopher, mutex-protected forks.
- **`philo_bonus/`** — bonus. One process per philosopher, forks are tokens taken from a POSIX semaphore.

## Features

- No data races, no deadlocks — even with 200 philosophers.
- Strict death detection: a philosopher's death is reported within a few milliseconds of the timeout.
- Compiles with `-Wall -Wextra -Werror`, **norminette** clean.
- No memory leaks, no rogue threads or zombie processes.

## Requirements

- A POSIX system (Linux / macOS).
- `cc` (or `gcc` / `clang`), `make`.
- pthreads, POSIX semaphores (`<semaphore.h>`) for the bonus part.

## Build

```sh
# Mandatory part
cd philo && make

# Bonus part
cd philo_bonus && make
```

Both binaries are produced at the root of their directory: `philo/philo` and `philo_bonus/philo_bonus`.

Other rules: `make clean`, `make fclean`, `make re`, `make norminette`.

## Usage

```
./philo philo_nbr time_to_die time_to_eat time_to_sleep [meals_per_philo]
```

| Argument | Meaning |
|---|---|
| `philo_nbr` | Number of philosophers (and forks). `1`–`200`. |
| `time_to_die` | Milliseconds without a meal before a philosopher dies. |
| `time_to_eat` | Milliseconds spent eating (holding both forks). |
| `time_to_sleep` | Milliseconds spent sleeping after a meal. |
| `meals_per_philo` | *Optional.* Simulation stops once every philosopher has eaten this many times. |

All values are strictly positive integers. `philo_nbr` must be at least 1.

### Examples

```sh
# 5 philosophers, nobody dies
./philo 5 800 200 200

# 4 philosophers, stop after each ate 7 times
./philo 4 410 200 200 7

# Tight timing — at least one philosopher dies
./philo 4 310 200 100

# Bonus part — same arguments
./philo_bonus 5 800 200 200
```

### Output

Each event is logged with a timestamp (ms since simulation start) and the philosopher id:

```
0 1 has taken a fork
0 1 has taken a fork
0 1 is eating
200 1 is sleeping
400 1 is thinking
...
812 3 died
```

After a death, no further log lines are printed.

## Architecture

### `philo/` — threads & mutexes

- One `pthread_t` per philosopher.
- One `pthread_mutex_t` per fork. A philosopher locks left then right (lower-id first to avoid the classic deadlock).
- A separate monitor checks `last_meal` timestamps and flips `simulation_end` when a philosopher starves or all meals are eaten.
- `stdout_lock` serializes log lines so death and activity messages never interleave.

### `philo_bonus/` — processes & semaphores

- One child process per philosopher (`fork()`).
- Forks are represented by a single counting semaphore initialized to `philo_nbr`; `sem_wait` takes a fork, `sem_post` releases it.
- Each child runs two threads: the eating routine and a death checker watching its own `last_meal`.
- When any philosopher dies or the meal quota is reached, the parent posts on `end_simu` and kills the surviving children.

> [!NOTE]
> The bonus uses **named** POSIX semaphores (`sem_open`). They live under `/dev/shm` on Linux. If a previous run crashed, stale semaphores can be cleaned with `rm /dev/shm/sem.philo_*`.

## Testing tips

```sh
# Stress test — should run forever without deaths
./philo 200 800 200 200

# Race-condition smoke test — must never log after "died"
./philo 4 310 200 100

# Valgrind / helgrind for the mandatory part
valgrind --tool=helgrind ./philo 5 800 200 200
```

> [!TIP]
> Useful constraints to remember: `time_to_die >= time_to_eat + time_to_sleep` is *not* guaranteed — the simulation must still detect deaths correctly when timings are tight.

## Project layout

```
.
├── philo/              # Mandatory — threads + mutexes
│   ├── includes/philo.h
│   ├── src/
│   │   ├── main.c
│   │   ├── init_simulation.c
│   │   ├── philo.c
│   │   └── utils.c
│   └── Makefile
└── philo_bonus/        # Bonus — processes + semaphores
    ├── includes/philo_bonus.h
    ├── src/
    │   ├── main.c
    │   ├── init_simulation.c
    │   ├── philo.c
    │   ├── philo_thread.c
    │   ├── philo_utils.c
    │   └── utils.c
    └── Makefile
```
