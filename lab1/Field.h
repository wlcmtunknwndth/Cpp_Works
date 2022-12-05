class Field {
    int W, H;
    char** field;

public:
    Field(){}

    Field(int W, int H) {
        this->W = W;
        this->H = H;
        field = new char* [W];
        for (int i = 0; i < W; i++) {
            field[i] = new char [H];
        }
    }

    void fullfill() {
        for (int i = 0; i < W; i++) {
            for (int j = 0; j < H; j++) {
                int rng = rand() % 100;
                if (rng < 60) {
                    field[i][j] = '_';
                }
                else if (rng < 70) {
                    field[i][j] = '$';
                }
                else if (rng < 85) {
                    field[i][j] = '0';
                }
                else {
                    field[i][j] = '*';
                }
            }
        }
    }

    void output() {
        cout << endl;
        for (int i = 0; i < W; i++) {
            for (int j = 0; j < H; j++) {
                cout << field[i][j] << " ";
            }
            cout << endl;
        }
    }

    bool isLegalMove(int x, int y) {
        if (x < 0 || x >= W || y < 0 || y >= H) {
            cout << endl << "\nOut of polygon\n" << endl;
            return false;
        }
        
        return field[x][y] != '0';
    }

    void newCh(int x, int y, char K)
    {
        this->field[x][y] = K;
    }

    char getCoordinate(int x, int y) {
        return this->field[x][y];
    }
};