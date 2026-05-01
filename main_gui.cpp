#include "Renderer.h"
#include "position.h"
#include "movegen.h"
#include "search.h"
#include "helpers.h"
#include <optional>
#include <vector>
#include <thread>
#include <atomic>
#include <mutex>

constexpr int  SEARCH_DEPTH = 5; // increase for stronger play
constexpr bool HUMAN_IS_WHITE = true; // human plays white

int main() {
    Renderer renderer;
    if (!renderer.init()) return -1;

    Position pos;
    pos.setStartingPosition();

    Search search;

    std::optional<Square> selected;
    std::vector<Square>   highlights;

    Colour humanColour = HUMAN_IS_WHITE ? Colour::White : Colour::Black;

    while (renderer.isOpen()) {
        if (!renderer.pollEvents()) break;

        // ── Engine turn ───────────────────────────────────────────────
        if (pos.sideToMove != humanColour) {
            SearchResult result = search.findBestMove(pos, SEARCH_DEPTH);

            if (!result.bestMove.isNull()) {
                Piece   moving = pos.pieceOn(result.bestMove.from());
                uint8_t prevC = pos.castlingRights;
                Square  prevE = pos.enPassantSquare;
                int     prevH = pos.halfMoveClock;
                pos.makeMove(result.bestMove);
            }
            selected.reset();
            highlights.clear();
        }

        // ── Human turn ────────────────────────────────────────────────
        if (renderer.lastClick.has_value() &&
            pos.sideToMove == humanColour) {

            Square clicked = renderer.lastClick.value();

            if (selected.has_value()) {
                Square from = selected.value();


                if (tryMove(pos, from, clicked)) {
                    selected.reset();
                    highlights.clear();
                }
                else {
                    // Reselect if clicking own piece
                    Piece p = pos.pieceOn(clicked);
                    if (!p.isEmpty() && p.colour == humanColour) {
                        selected = clicked;
                        highlights = legalDestinations(pos, clicked);
                    }
                    else {
                        selected.reset();
                        highlights.clear();
                    }
                }
            }
            else {
                // Select a piece
                Piece p = pos.pieceOn(clicked);
                if (!p.isEmpty() && p.colour == humanColour) {
                    selected = clicked;
                    highlights = legalDestinations(pos, clicked);
                }
            }
        }

        renderer.render(pos, selected, highlights);
    }

    return 0;
}