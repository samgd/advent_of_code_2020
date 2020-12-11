#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using std::istream;
using std::ostream;
using std::string;
using std::vector;

class SeatMap {
public:
    enum class SeatStatus{
        EMPTY, OCCUPIED, FLOOR
    };

    // Simulates a single round of changes and returns true if seats change.
    bool step(bool first_seat = false, int min_occupied = 4);

    int n_equal(const SeatStatus seat_status) const {
        int total = 0;
        for (auto i = 0; i < n_rows; ++i) {
            for (auto j = 0; j < n_cols; ++j) {
                total += status[i*n_cols + j] == seat_status;
            }
        }
        return total;
    }

    friend istream& operator>>(istream&, SeatMap&);
    friend ostream& operator<<(ostream&, const SeatMap&);
private:
    int n_steps;
    int n_rows;
    int n_cols;
    vector<SeatStatus> status;

    int n_occupied(int row, int col, bool first_seat) const
    {
        int n_occ = 0;
        for (auto di = -1; di <= 1; di++) {
            for (auto dj = -1; dj <= 1; dj++) {
                if (di == 0 && dj == 0) {
                    continue;
                }
                auto i = row + di;
                auto j = col + dj;

                if (i < 0 || i >= n_rows || j < 0 || j >= n_cols) {
                    continue;
                }

                auto seat = status[i*n_cols + j];
                while (first_seat && seat == SeatStatus::FLOOR) {
                    i += di;
                    j += dj;
                    if (i < 0 || i >= n_rows || j < 0 || j >= n_cols) {
                        break;
                    }
                    seat = status[i*n_cols + j];
                }

                n_occ += seat == SeatStatus::OCCUPIED;
            }
        }
        return n_occ;
    }
};

istream& operator>>(istream& is, SeatMap& sm)
{
    int n_rows = 0;
    int n_cols = -1;
    vector<SeatMap::SeatStatus> status;

    string line;

    while (getline(is, line)) {
        if (n_cols == -1) {
            n_cols = line.size();
        } else if (static_cast<int>(line.size()) != n_cols) {
            is.clear(std::ios::failbit);
            return is;
        }

        n_rows++;

        for (auto c : line) {
            switch (c) {
            case 'L':
                status.push_back(SeatMap::SeatStatus::EMPTY);
                break;
            case '#':
                status.push_back(SeatMap::SeatStatus::OCCUPIED);
                break;
            case '.':
                status.push_back(SeatMap::SeatStatus::FLOOR);
                break;
            default:
                is.clear(std::ios::failbit);
                return is;
            };
        }
    }

    sm.n_steps = 0;
    sm.n_rows = n_rows;
    sm.n_cols = n_cols;
    sm.status = status;

    return is;
}

ostream& operator<<(ostream& os, const SeatMap& sm)
{
    for (auto n_rows = 0; n_rows < sm.n_rows; n_rows++) {
        for (auto n_cols = 0; n_cols < sm.n_cols; n_cols++) {
            switch (sm.status[n_rows*sm.n_cols + n_cols]) {
            case SeatMap::SeatStatus::EMPTY:
                os << 'L';
                break;
            case SeatMap::SeatStatus::OCCUPIED:
                os << '#';
                break;
            case SeatMap::SeatStatus::FLOOR:
                os << '.';
                break;
            default:
                os.clear(std::ios::failbit);
            };
        }
        if (n_rows < sm.n_rows - 1) {
            os << '\n';
        }
    }

    return os;
}

bool SeatMap::step(bool first_seat, int min_occupied)
{
    bool seat_change = false;
    vector<SeatStatus> new_status{status};

    for (auto i = 0; i < n_rows; i++) {
        for (auto j = 0; j < n_cols; j++) {
            // updating seat (i, j)
            switch (status[i*n_cols + j]) {
            case SeatStatus::EMPTY:
                if (n_occupied(i, j, first_seat) == 0) {
                    seat_change = true;
                    new_status[i*n_cols + j] = SeatStatus::OCCUPIED;
                }
                break;
            case SeatStatus::OCCUPIED:
                if (n_occupied(i, j, first_seat) >= min_occupied) {
                    seat_change = true;
                    new_status[i*n_cols + j] = SeatStatus::EMPTY;
                }
                break;
            case SeatStatus::FLOOR:
                break;
            }
        }
    }

    status = new_status;
    return seat_change;
}

int main()
{
    // Part 1
    std::ifstream data{"input.txt"};
    SeatMap sm;
    data >> sm;
    while (sm.step(false, 4));
    std::cout << sm.n_equal(SeatMap::SeatStatus::OCCUPIED) << std::endl;

    // Part 2
    data.clear();
    data.seekg(0);
    data >> sm;
    while (sm.step(true, 5));
    std::cout << sm.n_equal(SeatMap::SeatStatus::OCCUPIED) << std::endl;
}
