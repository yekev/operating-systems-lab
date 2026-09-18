#include <atomic>
#include <cerrno>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <fcntl.h>
#include <pthread.h>
#include <termios.h>
#include <thread>
#include <unistd.h>

namespace {

constexpr int kRows = 17;
constexpr int kColumns = 49;
constexpr int kObjectCount = 6;
constexpr int kWallLength = 15;
constexpr char kHorizontalLine = '-';
constexpr char kVerticalLine = '|';
constexpr char kCorner = '+';
constexpr char kPlayer = '0';
constexpr char kWall = '=';
constexpr char kGold = '$';

struct GameObject {
    int row;
    int column;
    int direction;
    bool active;
};

int player_row = 0;
int player_column = 0;
int previous_player_row = 0;
int previous_player_column = 0;
int gold_collected = 0;
char board[kRows][kColumns + 1]{};
GameObject walls[kObjectCount]{};
GameObject gold[kObjectCount]{};

std::atomic_bool game_running{true};
std::atomic_bool game_won{false};
std::atomic_bool game_lost{false};
pthread_mutex_t game_mutex = PTHREAD_MUTEX_INITIALIZER;

class MutexGuard {
public:
    explicit MutexGuard(pthread_mutex_t *mutex) : mutex_(mutex)
    {
        pthread_mutex_lock(mutex_);
    }

    ~MutexGuard()
    {
        pthread_mutex_unlock(mutex_);
    }

    MutexGuard(const MutexGuard &) = delete;
    MutexGuard &operator=(const MutexGuard &) = delete;

private:
    pthread_mutex_t *mutex_;
};

class TerminalSession {
public:
    TerminalSession()
    {
        if (!isatty(STDIN_FILENO)) {
            return;
        }

        if (tcgetattr(STDIN_FILENO, &original_termios_) == -1) {
            return;
        }

        original_flags_ = fcntl(STDIN_FILENO, F_GETFL, 0);
        if (original_flags_ == -1) {
            return;
        }

        struct termios raw = original_termios_;
        raw.c_lflag &= static_cast<tcflag_t>(~(ICANON | ECHO));
        raw.c_cc[VMIN] = 0;
        raw.c_cc[VTIME] = 0;

        if (tcsetattr(STDIN_FILENO, TCSANOW, &raw) == -1 ||
            fcntl(STDIN_FILENO, F_SETFL, original_flags_ | O_NONBLOCK) == -1) {
            tcsetattr(STDIN_FILENO, TCSANOW, &original_termios_);
            return;
        }

        active_ = true;
        std::printf("\033[?25l");
        std::fflush(stdout);
    }

    ~TerminalSession()
    {
        if (active_) {
            tcsetattr(STDIN_FILENO, TCSANOW, &original_termios_);
            fcntl(STDIN_FILENO, F_SETFL, original_flags_);
        }
        std::printf("\033[?25h");
        std::fflush(stdout);
    }

    TerminalSession(const TerminalSession &) = delete;
    TerminalSession &operator=(const TerminalSession &) = delete;

private:
    struct termios original_termios_{};
    int original_flags_ = 0;
    bool active_ = false;
};

int wrap_column(int column)
{
    const int width = kColumns - 2;
    while (column < 1) {
        column += width;
    }
    while (column >= kColumns - 1) {
        column -= width;
    }
    return column;
}

void initialize_game(unsigned int seed)
{
    std::srand(seed);
    std::memset(board, 0, sizeof(board));

    for (int row = 1; row < kRows - 1; ++row) {
        for (int column = 1; column < kColumns - 1; ++column) {
            board[row][column] = ' ';
        }
    }

    for (int column = 1; column < kColumns - 1; ++column) {
        board[0][column] = kHorizontalLine;
        board[kRows - 1][column] = kHorizontalLine;
    }
    for (int row = 1; row < kRows - 1; ++row) {
        board[row][0] = kVerticalLine;
        board[row][kColumns - 1] = kVerticalLine;
    }

    board[0][0] = kCorner;
    board[0][kColumns - 1] = kCorner;
    board[kRows - 1][0] = kCorner;
    board[kRows - 1][kColumns - 1] = kCorner;

    player_row = kRows / 2;
    player_column = kColumns / 2;
    previous_player_row = player_row;
    previous_player_column = player_column;

    const int wall_rows[kObjectCount] = {2, 4, 6, 10, 12, 14};
    const int wall_directions[kObjectCount] = {1, -1, 1, -1, 1, -1};
    for (int index = 0; index < kObjectCount; ++index) {
        walls[index] = {
            wall_rows[index],
            std::rand() % (kColumns - kWallLength - 2) + 1,
            wall_directions[index],
            true,
        };
    }

    const int gold_rows[kObjectCount] = {1, 3, 5, 11, 13, 15};
    for (int index = 0; index < kObjectCount; ++index) {
        gold[index] = {
            gold_rows[index],
            std::rand() % (kColumns - 2) + 1,
            (std::rand() % 2 == 0) ? -1 : 1,
            true,
        };
    }
}

void update_board()
{
    if (previous_player_row != player_row || previous_player_column != player_column) {
        board[previous_player_row][previous_player_column] = ' ';
    }

    for (int row = 1; row < kRows - 1; ++row) {
        for (int column = 1; column < kColumns - 1; ++column) {
            if (board[row][column] == kWall || board[row][column] == kGold) {
                board[row][column] = ' ';
            }
        }
    }

    for (const GameObject &wall : walls) {
        if (!wall.active) {
            continue;
        }
        for (int offset = 0; offset < kWallLength; ++offset) {
            board[wall.row][wrap_column(wall.column + offset)] = kWall;
        }
    }

    for (const GameObject &piece : gold) {
        if (piece.active) {
            board[piece.row][wrap_column(piece.column)] = kGold;
        }
    }

    board[player_row][player_column] = kPlayer;
    previous_player_row = player_row;
    previous_player_column = player_column;
}

void check_collisions()
{
    for (const GameObject &wall : walls) {
        if (!wall.active || player_row != wall.row) {
            continue;
        }
        for (int offset = 0; offset < kWallLength; ++offset) {
            if (player_column == wrap_column(wall.column + offset)) {
                game_lost.store(true);
                game_running.store(false);
                return;
            }
        }
    }

    for (GameObject &piece : gold) {
        if (piece.active && player_row == piece.row &&
            player_column == wrap_column(piece.column)) {
            piece.active = false;
            ++gold_collected;
            if (gold_collected == kObjectCount) {
                game_won.store(true);
                game_running.store(false);
                return;
            }
        }
    }
}

void render_board()
{
    std::printf("\033[H\033[2J");
    for (const auto &row : board) {
        std::puts(row);
    }
    std::printf("Gold: %d/%d | W/A/S/D: move | Q: quit\n", gold_collected, kObjectCount);
    std::fflush(stdout);
}

void *input_worker(void *)
{
    while (game_running.load()) {
        char input = '\0';
        ssize_t bytes_read = read(STDIN_FILENO, &input, 1);
        if (bytes_read == 1) {
            MutexGuard lock(&game_mutex);
            int next_row = player_row;
            int next_column = player_column;

            switch (input) {
            case 'w': case 'W': --next_row; break;
            case 's': case 'S': ++next_row; break;
            case 'a': case 'A': --next_column; break;
            case 'd': case 'D': ++next_column; break;
            case 'q': case 'Q': game_running.store(false); break;
            default: break;
            }

            if (next_row > 0 && next_row < kRows - 1 &&
                next_column > 0 && next_column < kColumns - 1) {
                player_row = next_row;
                player_column = next_column;
                check_collisions();
                update_board();
            }
        } else if (bytes_read == -1 && errno != EAGAIN && errno != EWOULDBLOCK) {
            game_running.store(false);
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    return nullptr;
}

void *wall_worker(void *)
{
    while (game_running.load()) {
        {
            MutexGuard lock(&game_mutex);
            if (!game_running.load()) {
                break;
            }
            for (GameObject &wall : walls) {
                wall.column += wall.direction;
                if (wall.column < 1 - kWallLength) {
                    wall.column += kColumns - 2;
                } else if (wall.column >= kColumns - 1) {
                    wall.column -= kColumns - 2;
                }
            }
            check_collisions();
            update_board();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
    return nullptr;
}

void *gold_worker(void *)
{
    while (game_running.load()) {
        {
            MutexGuard lock(&game_mutex);
            if (!game_running.load()) {
                break;
            }
            for (GameObject &piece : gold) {
                if (piece.active) {
                    piece.column = wrap_column(piece.column + piece.direction);
                }
            }
            check_collisions();
            update_board();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(150));
    }
    return nullptr;
}

void stop_and_join(pthread_t *threads, int count)
{
    game_running.store(false);
    for (int index = 0; index < count; ++index) {
        pthread_join(threads[index], nullptr);
    }
}

} // namespace

int main(int argc, char *argv[])
{
    unsigned int seed = static_cast<unsigned int>(std::time(nullptr));
    if (argc == 3 && std::strcmp(argv[1], "--seed") == 0) {
        seed = static_cast<unsigned int>(std::strtoul(argv[2], nullptr, 10));
    } else if (argc != 1) {
        std::fprintf(stderr, "Usage: %s [--seed NUMBER]\n", argv[0]);
        return EXIT_FAILURE;
    }

    TerminalSession terminal;
    initialize_game(seed);
    update_board();

    pthread_t threads[3]{};
    void *(*workers[3])(void *) = {input_worker, wall_worker, gold_worker};
    int created = 0;
    for (; created < 3; ++created) {
        int error = pthread_create(&threads[created], nullptr, workers[created], nullptr);
        if (error != 0) {
            std::fprintf(stderr, "pthread_create: %s\n", std::strerror(error));
            stop_and_join(threads, created);
            pthread_mutex_destroy(&game_mutex);
            return EXIT_FAILURE;
        }
    }

    while (game_running.load()) {
        {
            MutexGuard lock(&game_mutex);
            render_board();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    stop_and_join(threads, created);
    pthread_mutex_destroy(&game_mutex);

    std::printf("\033[H\033[2J");
    if (game_won.load()) {
        std::puts("You win the game!");
    } else if (game_lost.load()) {
        std::puts("You lose the game.");
    } else {
        std::puts("You exit the game.");
    }

    return EXIT_SUCCESS;
}
