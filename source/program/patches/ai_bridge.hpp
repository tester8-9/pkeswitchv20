#pragma once

// AI Bridge V26 FIXED - fresh-board switch scorer.
// Goals:
// - use the real switch-candidate rows the engine already creates;
// - ignore stale target records after the player switches;
// - preserve only the active Pokémon's opening Fake Out once;
// - allow the other slot to switch even if its partner has Fake Out;
// - delay switching for active setup/priority only when that active Pokémon itself appears to have value.

#include "lib.hpp"
#include "external.hpp"
#include "symbols.hpp"
#include "config.hpp"
#include "loggers.hpp"

namespace AIBridge {
    inline bool installed = false;
    inline u32 readback_hits = 0;
    inline u32 candidate_hits = 0;
    inline u32 policy_forced_total = 0;
    inline bool move_records_consumed = false;
    inline u32 move_generation = 0;
    inline bool opening_fakeout_guard_consumed = false;

    struct MoveRecord {
        u32 move_id;
        u16 target_species;
        s32 sum;
        s32 min;
        s32 max;
        u32 count;
    };
    inline MoveRecord move_records[32] = {};
    inline u32 move_record_count = 0;
    inline u16 recent_targets[4] = {0,0,0,0};
    inline u32 recent_target_count = 0;
    inline u32 recent_target_generation = 0;

    enum Type : u8 {
        BT_NORMAL=0, BT_FIGHTING=1, BT_FLYING=2, BT_POISON=3, BT_GROUND=4, BT_ROCK=5,
        BT_BUG=6, BT_GHOST=7, BT_STEEL=8, BT_FIRE=9, BT_WATER=10, BT_GRASS=11,
        BT_ELECTRIC=12, BT_PSYCHIC=13, BT_ICE=14, BT_DRAGON=15, BT_DARK=16, BT_FAIRY=17,
        BT_NONE=0xFF
    };
    struct SpeciesTypes { u8 t1; u8 t2; };
    struct TeamDef { u16 trainer_id; u8 count; u16 species[6]; };

    static inline bool ShouldLog(u32 hit) {
        return global_config.ai_bridge.log_hits && hit <= global_config.ai_bridge.max_log_hits;
    }
    static inline bool LooksLikePtr(u64 p) { return p >= 0x100000; }
    static inline bool LooksLikeHeapPtr(u64 p) { return p >= 0x2000000000ULL && p < 0x2400000000ULL; }
    static inline u8 ReadU8(u64 p, u64 off) { return *reinterpret_cast<volatile u8*>(p + off); }
    static inline u16 ReadU16(u64 p, u64 off) { return *reinterpret_cast<volatile u16*>(p + off); }
    static inline u32 ReadU32(u64 p, u64 off) { return *reinterpret_cast<volatile u32*>(p + off); }
    static inline u64 PtrHighLow(u64 p, u64 off) {
        const u32 hi = ReadU32(p, off);
        const u32 lo = ReadU32(p, off + 4);
        return (static_cast<u64>(hi) << 32) | static_cast<u64>(lo);
    }
    static inline u64 PtrLowHigh(u64 p, u64 off) {
        const u32 lo = ReadU32(p, off);
        const u32 hi = ReadU32(p, off + 4);
        return (static_cast<u64>(hi) << 32) | static_cast<u64>(lo);
    }

    static inline const char* TypeName(u8 t) {
        switch (t) {
            case BT_NORMAL: return "Normal"; case BT_FIGHTING: return "Fighting"; case BT_FLYING: return "Flying";
            case BT_POISON: return "Poison"; case BT_GROUND: return "Ground"; case BT_ROCK: return "Rock";
            case BT_BUG: return "Bug"; case BT_GHOST: return "Ghost"; case BT_STEEL: return "Steel";
            case BT_FIRE: return "Fire"; case BT_WATER: return "Water"; case BT_GRASS: return "Grass";
            case BT_ELECTRIC: return "Electric"; case BT_PSYCHIC: return "Psychic"; case BT_ICE: return "Ice";
            case BT_DRAGON: return "Dragon"; case BT_DARK: return "Dark"; case BT_FAIRY: return "Fairy";
            default: return "None";
        }
    }
    static inline const char* KnownSpeciesName(u16 s) {
        switch (s) {
            case 448: return "Lucario";
            case 475: return "Gallade";
            case 534: return "Conkeldurr";
            case 620: return "Mienshao";
            case 675: return "Pangoro";
            case 815: return "Cinderace";
            case 861: return "Grimmsnarl";
            case 869: return "Alcremie";
            case 858: return "Hatterene";
            case 823: return "Corviknight";
            case 812: return "Rillaboom";
            case 887: return "Dragapult";
            case 778: return "Mimikyu";
            case 530: return "Excadrill";
            case 248: return "Tyranitar";
            case 468: return "Togekiss";
            case 876: return "Indeedee";
            default: return "";
        }
    }
    static inline SpeciesTypes Types(u16 s) {
        switch (s) {
            case 448: return {BT_FIGHTING, BT_STEEL};
            case 475: return {BT_PSYCHIC, BT_FIGHTING};
            case 534: return {BT_FIGHTING, BT_NONE};
            case 620: return {BT_FIGHTING, BT_NONE};
            case 675: return {BT_FIGHTING, BT_DARK};
            case 815: return {BT_FIRE, BT_NONE};
            case 861: return {BT_DARK, BT_FAIRY};
            case 869: return {BT_FAIRY, BT_NONE};
            case 858: return {BT_PSYCHIC, BT_FAIRY};
            case 823: return {BT_FLYING, BT_STEEL};
            case 812: return {BT_GRASS, BT_NONE};
            case 887: return {BT_DRAGON, BT_GHOST};
            case 778: return {BT_GHOST, BT_FAIRY};
            case 530: return {BT_GROUND, BT_STEEL};
            case 248: return {BT_ROCK, BT_DARK};
            case 468: return {BT_FAIRY, BT_FLYING};
            case 876: return {BT_PSYCHIC, BT_NORMAL};
            default: return {BT_NONE, BT_NONE};
        }
    }
    static inline bool HasTypes(SpeciesTypes t) { return t.t1 != BT_NONE; }
    static inline bool HasType(SpeciesTypes t, u8 x) { return t.t1 == x || t.t2 == x; }

    // Effectiveness in quarter-units: 0, 1=0.25x, 2=0.5x, 4=1x, 8=2x, 16=4x.
    static inline u8 OneTypeQ4(u8 a, u8 d) {
        if (a == BT_NONE || d == BT_NONE) return 4;
        switch (a) {
            case BT_NORMAL: if (d==BT_ROCK||d==BT_STEEL) return 2; if (d==BT_GHOST) return 0; return 4;
            case BT_FIGHTING: if (d==BT_NORMAL||d==BT_ROCK||d==BT_STEEL||d==BT_ICE||d==BT_DARK) return 8; if (d==BT_FLYING||d==BT_POISON||d==BT_BUG||d==BT_PSYCHIC||d==BT_FAIRY) return 2; if (d==BT_GHOST) return 0; return 4;
            case BT_FLYING: if (d==BT_FIGHTING||d==BT_BUG||d==BT_GRASS) return 8; if (d==BT_ROCK||d==BT_STEEL||d==BT_ELECTRIC) return 2; return 4;
            case BT_POISON: if (d==BT_GRASS||d==BT_FAIRY) return 8; if (d==BT_POISON||d==BT_GROUND||d==BT_ROCK||d==BT_GHOST) return 2; if (d==BT_STEEL) return 0; return 4;
            case BT_GROUND: if (d==BT_POISON||d==BT_ROCK||d==BT_STEEL||d==BT_FIRE||d==BT_ELECTRIC) return 8; if (d==BT_BUG||d==BT_GRASS) return 2; if (d==BT_FLYING) return 0; return 4;
            case BT_ROCK: if (d==BT_FLYING||d==BT_BUG||d==BT_FIRE||d==BT_ICE) return 8; if (d==BT_FIGHTING||d==BT_GROUND||d==BT_STEEL) return 2; return 4;
            case BT_BUG: if (d==BT_GRASS||d==BT_PSYCHIC||d==BT_DARK) return 8; if (d==BT_FIGHTING||d==BT_FLYING||d==BT_POISON||d==BT_GHOST||d==BT_STEEL||d==BT_FIRE||d==BT_FAIRY) return 2; return 4;
            case BT_GHOST: if (d==BT_GHOST||d==BT_PSYCHIC) return 8; if (d==BT_DARK) return 2; if (d==BT_NORMAL) return 0; return 4;
            case BT_STEEL: if (d==BT_ROCK||d==BT_ICE||d==BT_FAIRY) return 8; if (d==BT_STEEL||d==BT_FIRE||d==BT_WATER||d==BT_ELECTRIC) return 2; return 4;
            case BT_FIRE: if (d==BT_BUG||d==BT_STEEL||d==BT_GRASS||d==BT_ICE) return 8; if (d==BT_ROCK||d==BT_FIRE||d==BT_WATER||d==BT_DRAGON) return 2; return 4;
            case BT_WATER: if (d==BT_GROUND||d==BT_ROCK||d==BT_FIRE) return 8; if (d==BT_WATER||d==BT_GRASS||d==BT_DRAGON) return 2; return 4;
            case BT_GRASS: if (d==BT_GROUND||d==BT_ROCK||d==BT_WATER) return 8; if (d==BT_FLYING||d==BT_POISON||d==BT_BUG||d==BT_STEEL||d==BT_FIRE||d==BT_GRASS||d==BT_DRAGON) return 2; return 4;
            case BT_ELECTRIC: if (d==BT_FLYING||d==BT_WATER) return 8; if (d==BT_GRASS||d==BT_ELECTRIC||d==BT_DRAGON) return 2; if (d==BT_GROUND) return 0; return 4;
            case BT_PSYCHIC: if (d==BT_FIGHTING||d==BT_POISON) return 8; if (d==BT_STEEL||d==BT_PSYCHIC) return 2; if (d==BT_DARK) return 0; return 4;
            case BT_ICE: if (d==BT_FLYING||d==BT_GROUND||d==BT_GRASS||d==BT_DRAGON) return 8; if (d==BT_STEEL||d==BT_FIRE||d==BT_WATER||d==BT_ICE) return 2; return 4;
            case BT_DRAGON: if (d==BT_DRAGON) return 8; if (d==BT_STEEL) return 2; if (d==BT_FAIRY) return 0; return 4;
            case BT_DARK: if (d==BT_GHOST||d==BT_PSYCHIC) return 8; if (d==BT_FIGHTING||d==BT_DARK||d==BT_FAIRY) return 2; return 4;
            case BT_FAIRY: if (d==BT_FIGHTING||d==BT_DRAGON||d==BT_DARK) return 8; if (d==BT_POISON||d==BT_STEEL||d==BT_FIRE) return 2; return 4;
            default: return 4;
        }
    }
    static inline u8 EffectQ4(u8 a, SpeciesTypes d) {
        if (d.t1 == BT_NONE) return 4;
        u32 r = OneTypeQ4(a, d.t1);
        if (d.t2 != BT_NONE) r = (r * OneTypeQ4(a, d.t2)) / 4;
        return static_cast<u8>(r);
    }
    static inline u8 BestStabQ4(u16 attacker, u16 defender) {
        const SpeciesTypes at = Types(attacker); const SpeciesTypes df = Types(defender);
        if (!HasTypes(at) || !HasTypes(df)) return 4;
        u8 best = EffectQ4(at.t1, df);
        if (at.t2 != BT_NONE) { const u8 b = EffectQ4(at.t2, df); if (b > best) best = b; }
        return best;
    }

    // Libero/Protean-style threat profiles: score likely move pressure, not just current/base type.
    static inline u8 ThreatProfileMaxQ4(u16 threat, SpeciesTypes defender) {
        if (!HasTypes(defender)) return 4;
        u8 best = 4;
        auto consider = [&](u8 t){ const u8 e = EffectQ4(t, defender); if (e > best) best = e; };
        const SpeciesTypes st = Types(threat);
        if (st.t1 != BT_NONE) consider(st.t1); if (st.t2 != BT_NONE) consider(st.t2);
        if (threat == 815) { consider(BT_FIRE); consider(BT_STEEL); consider(BT_FIGHTING); consider(BT_DARK); consider(BT_NORMAL); }
        if (threat == 658) { consider(BT_WATER); consider(BT_DARK); consider(BT_ICE); consider(BT_POISON); consider(BT_GRASS); }
        if (threat == 861) { consider(BT_DARK); consider(BT_FAIRY); }
        if (threat == 869) { consider(BT_FAIRY); consider(BT_PSYCHIC); }
        return best;
    }

    static inline u16 SpeciesAtObject(u64 ptr) {
        if (!LooksLikePtr(ptr)) return 0;
        const u16 a = ReadU16(ptr, 0x70); if (a == 448||a==475||a==534||a==620||a==675||a==815||a==861||a==869||a==858) return a;
        const u16 b = ReadU16(ptr, 0x38); if (b == 448||b==475||b==534||b==620||b==675||b==815||b==861||b==869||b==858) return b;
        return a; // harmless for logging; unknown is handled conservatively.
    }
    static inline u16 EvalTargetSpecies(u64 ptr) { return LooksLikePtr(ptr) ? ReadU16(ptr, 0x70) : 0; }
    static inline u16 CandidateActiveSpecies(u64 state) { return SpeciesAtObject(PtrHighLow(state, 0x80)); }
    static inline u16 CandidateFallbackSwitchSpecies(u64 state) { return SpeciesAtObject(PtrHighLow(state, 0x78)); }

    static constexpr TeamDef kTeams[] = {
        {62, 5, {620,475,534,448,675,0}},
        {63, 5, {620,475,534,448,675,0}},
    };
    static inline const TeamDef* ResolveTeam(u16 active, u16 fallback) {
        const TeamDef* best = nullptr; s32 score_best = -9999;
        for (u32 i=0; i<sizeof(kTeams)/sizeof(kTeams[0]); i++) {
            s32 sc = 0;
            for (u32 j=0; j<6; j++) { if (kTeams[i].species[j] == active && active) sc += 80; if (kTeams[i].species[j] == fallback && fallback) sc += 40; }
            if (sc > score_best) { score_best = sc; best = &kTeams[i]; }
        }
        return score_best >= 60 ? best : nullptr;
    }
    static inline u16 InferredSwitchSpecies(u64 state, u32 action) {
        const u16 active = CandidateActiveSpecies(state);
        const u16 fallback = CandidateFallbackSwitchSpecies(state);
        const TeamDef* team = ResolveTeam(active, fallback);
        if (team && action < 6 && team->species[action] != 0) return team->species[action];
        return fallback;
    }
    static inline bool FriendlySpecies(u16 s, u16 active, u16 switchin) {
        if (s == 0 || s == active || s == switchin) return true;
        if (s == 620 || s == 475 || s == 534 || s == 448 || s == 675) return true;
        return false;
    }
    static inline void ClearMoveWindow() {
        for (u32 i=0; i<32; i++) move_records[i] = {};
        for (u32 i=0; i<4; i++) recent_targets[i] = 0;
        move_record_count = 0;
        recent_target_count = 0;
        move_generation++;
        move_records_consumed = false;
    }
    static inline void TrackRecentTarget(u16 species) {
        if (species == 0) return;
        if (recent_target_generation != move_generation) {
            recent_target_generation = move_generation;
            recent_target_count = 0;
            for (u32 i=0; i<4; i++) recent_targets[i] = 0;
        }
        for (u32 i=0; i<recent_target_count; i++) if (recent_targets[i] == species) return;
        if (recent_target_count < 4) recent_targets[recent_target_count++] = species;
    }
    static inline void AddMoveRecord(u32 move, u16 target, s32 delta) {
        if (target) TrackRecentTarget(target);
        for (u32 i=0; i<move_record_count; i++) {
            if (move_records[i].move_id == move && move_records[i].target_species == target) {
                move_records[i].sum += delta;
                if (delta < move_records[i].min) move_records[i].min = delta;
                if (delta > move_records[i].max) move_records[i].max = delta;
                move_records[i].count++;
                return;
            }
        }
        if (move_record_count < 32) {
            move_records[move_record_count++] = {move, target, delta, delta, delta, 1};
        }
    }
    static inline bool SummaryHasMove(u32 move) {
        for (u32 i=0; i<move_record_count; i++) if (move_records[i].move_id == move) return true;
        return false;
    }
    static inline bool SummaryHasPositiveMove(u32 move) {
        for (u32 i=0; i<move_record_count; i++) if (move_records[i].move_id == move && move_records[i].sum > 0) return true;
        return false;
    }
    static inline bool SummaryHasPositivePriority() {
        const u32 pri[] = {98,453,389,183,418,245,420,425,410,660,803,364};
        for (u32 i=0; i<sizeof(pri)/sizeof(pri[0]); i++) if (SummaryHasPositiveMove(pri[i])) return true;
        return false;
    }
    static inline s32 BestMoveScore() {
        s32 best = -9999;
        for (u32 i=0; i<move_record_count; i++) if (move_records[i].sum > best) best = move_records[i].sum;
        return best == -9999 ? 0 : best;
    }
    static inline s32 WorstMoveScore() {
        s32 worst = 9999;
        for (u32 i=0; i<move_record_count; i++) if (move_records[i].sum < worst) worst = move_records[i].sum;
        return worst == 9999 ? 0 : worst;
    }

    static inline s32 ScoreSwitch(u64 state, u32 action, const char* tag, u32 hit, u16* out_active, u16* out_switch) {
        const u16 active = CandidateActiveSpecies(state);
        const u16 switchin = InferredSwitchSpecies(state, action);
        if (out_active) *out_active = active; if (out_switch) *out_switch = switchin;
        const SpeciesTypes active_t = Types(active), switch_t = Types(switchin);
        const s32 best = BestMoveScore(); const s32 worst = WorstMoveScore();
        if (!HasTypes(active_t) || !HasTypes(switch_t)) return -9999;

        // True opening Fake Out guard: only Mienshao itself, only once, only early.
        if (active == 620 && SummaryHasMove(252) && !opening_fakeout_guard_consumed && move_generation <= 1) {
            opening_fakeout_guard_consumed = true;
            if (ShouldLog(hit)) Logging.Log("[ai_bridge] %s v26_score blocked: opening_fakeout_once active=%u/%s switchin=%u/%s gen=%u\n", tag, active, KnownSpeciesName(active), switchin, KnownSpeciesName(switchin), move_generation);
            return -9999;
        }

        s32 score = 0;
        if (best >= 6) score -= 70; else if (best >= 3) score -= 35; else if (best >= 1) score -= 15; else score += 10;
        if (worst <= -20) score += 12;

        // Delay active Pokémon with real setup/priority value, but allow a strong matchup score to overcome it.
        if (SummaryHasMove(433) && (active == 475 || active == 858)) score -= 55; // Trick Room on Gallade/Hatterene-like setters
        if (SummaryHasMove(366)) score -= 40; // Tailwind holder likely still has support value.
        if (SummaryHasPositivePriority()) score -= 35; // proxy for low-HP/clean-up pressure.

        u32 target_count = 0, bad = 0, better = 0, offense = 0, danger = 0;
        const bool use_recent = recent_target_count > 0;
        const u32 loop_count = use_recent ? recent_target_count : move_record_count;
        for (u32 i=0; i<loop_count; i++) {
            const u16 target = use_recent ? recent_targets[i] : move_records[i].target_species;
            if (FriendlySpecies(target, active, switchin)) continue;
            if (!HasTypes(Types(target))) continue;
            bool dup = false;
            if (!use_recent) { for (u32 j=0; j<i; j++) if (move_records[j].target_species == target) dup = true; }
            if (dup) continue;
            target_count++;
            const u8 tvs_act = ThreatProfileMaxQ4(target, active_t);
            const u8 tvs_sw = ThreatProfileMaxQ4(target, switch_t);
            const u8 sw_off = BestStabQ4(switchin, target);
            if (tvs_act >= 8) { score += 30; danger++; }
            if (tvs_sw == 0) { score += 50; better++; }
            else if (tvs_sw <= 2) { score += 35; better++; }
            else if (tvs_sw <= 4) { score += 8; }
            else if (tvs_sw >= 16) { score -= 120; bad++; }
            else if (tvs_sw >= 8) { score -= 50; bad++; }
            if (tvs_sw < tvs_act) { score += 35; better++; }
            else if (tvs_sw > tvs_act) { score -= 25; bad++; }
            if (sw_off >= 16) { score += 70; offense++; }
            else if (sw_off >= 8) { score += 45; offense++; }
            else if (sw_off == 0) score -= 35;

            // Board-specific V26 tuning.
            if (switchin == 448 && target == 869) score += 125; // Lucario is a major Steel answer to Alcremie.
            if (switchin == 448 && target == 861) score += 35;  // Lucario is acceptable into Grimmsnarl.
            if (switchin == 448 && target == 815) score -= 115; // but do not ignore Cinderace Fire/coverage.
            if ((active == 620 || active == 475 || active == 534 || active == 675) && target == 869 && switchin == 448) score += 70;
            if ((switchin == 675 || switchin == 475 || switchin == 534) && (target == 869 || target == 861)) score -= 80;
        }
        if (target_count == 0) score -= 60;
        if (bad >= 2) score -= 70;
        if (better >= 2) score += 25;
        if (offense >= 1 && better >= 1) score += 30;
        if (danger == 0 && best > 0) score -= 30;
        if (switchin == 448 && recent_target_count > 0) {
            bool has_alc=false, has_cin=false, has_grim=false;
            for (u32 i=0; i<recent_target_count; i++) { if (recent_targets[i]==869) has_alc=true; if (recent_targets[i]==815) has_cin=true; if (recent_targets[i]==861) has_grim=true; }
            if (has_alc && !has_cin) score += 90;
            if (has_alc && has_cin) score += 15; // mixed, not stale-veto.
            if (!has_alc && has_cin) score -= 90;
            if (has_grim && !has_cin) score += 20;
        }
        if (ShouldLog(hit)) {
            Logging.Log("[ai_bridge] %s v26_score action=%u active=%u/%s switchin=%u/%s score=%d best=%d worst=%d targets=%u recent=%u bad=%u better=%u offense=%u danger=%u gen=%u\n", tag, action, active, KnownSpeciesName(active), switchin, KnownSpeciesName(switchin), score, best, worst, target_count, recent_target_count, bad, better, offense, danger, move_generation);
        }
        return score;
    }

    static inline void DumpCandidates(const char* tag, u64 state, u32 hit) {
        if (!ShouldLog(hit) || !LooksLikePtr(state)) return;
        const u32 count = ReadU8(state, 0xC5);
        const u32 max = count > global_config.ai_bridge.candidate_dump_count ? global_config.ai_bridge.candidate_dump_count : count;
        for (u32 i=0; i<max && i<16; i++) {
            const u64 e = state + 0xC8 + static_cast<u64>(i) * 8;
            Logging.Log("[ai_bridge] %s cand[%u] action=%u enabled=%u score=%u\n", tag, i, static_cast<u32>(ReadU8(e,0)), static_cast<u32>(ReadU8(e,1)), ReadU32(e,4));
        }
    }

    static inline void ApplySwitchPolicy(u64 state, const char* tag, u32 hit) {
        if (global_config.ai_bridge.switch_policy_mode == 0) return;
        if (!LooksLikePtr(state)) return;
        if (hit < global_config.ai_bridge.switch_min_hit) return;
        const u32 count = ReadU8(state, 0xC5);
        if (count < global_config.ai_bridge.switch_require_candidate_count) return;
        if (global_config.ai_bridge.switch_gate_use_move_summary && move_record_count < global_config.ai_bridge.switch_gate_min_move_records) {
            if (ShouldLog(hit)) Logging.Log("[ai_bridge] %s v26_gate blocked: not_enough_records count=%u need=%u\n", tag, move_record_count, global_config.ai_bridge.switch_gate_min_move_records);
            return;
        }
        s32 best_score = -999999; u32 best_slot = 0xFFFFFFFF; u32 best_action = 0; u16 best_active = 0, best_switch = 0;
        for (u32 i=0; i<count && i<16; i++) {
            const u64 e = state + 0xC8 + static_cast<u64>(i) * 8;
            const u32 action = ReadU8(e,0);
            const u32 native = ReadU32(e,4);
            if (action < global_config.ai_bridge.force_existing_action_min || action > global_config.ai_bridge.force_existing_action_max) continue;
            if (native < global_config.ai_bridge.switch_min_native_score || native > global_config.ai_bridge.switch_max_native_score) continue;
            u16 active=0, sw=0;
            const s32 sc = ScoreSwitch(state, action, tag, hit, &active, &sw);
            if (sc > best_score) { best_score=sc; best_slot=i; best_action=action; best_active=active; best_switch=sw; }
        }
        const s32 threshold = 25;
        if (best_slot == 0xFFFFFFFF || best_score < threshold) {
            if (ShouldLog(hit)) Logging.Log("[ai_bridge] %s v26_gate blocked best_score=%d threshold=%d best_action=%u\n", tag, best_score, threshold, best_action);
            move_records_consumed = true;
            return;
        }
        const u64 e = state + 0xC8 + static_cast<u64>(best_slot) * 8;
        *reinterpret_cast<volatile u8*>(e + 1) = 1;
        u32 target = global_config.ai_bridge.switch_score;
        if (best_score >= 120) target = target < 128 ? 128 : target;
        if (best_score >= 200) target = target < 145 ? 145 : target;
        if (best_score >= 280) target = target < 165 ? 165 : target;
        if (best_score >= 360) target = target < 180 ? 180 : target;
        if (!global_config.ai_bridge.switch_native_score_only) *reinterpret_cast<volatile u32*>(e + 4) = target;
        policy_forced_total++;
        move_records_consumed = true;
        if (ShouldLog(hit)) {
            Logging.Log("[ai_bridge] %s switch_policy_v26 best_row changed=1 slot=%u action=%u active=%u/%s switchin=%u/%s best_score=%d target=%u recent_targets=%u gen=%u\n", tag, best_slot, best_action, best_active, KnownSpeciesName(best_active), best_switch, KnownSpeciesName(best_switch), best_score, target, recent_target_count, move_generation);
            DumpCandidates("candidate_score_after_switch_policy_v26", state, hit);
        }
    }
}

HOOK_DEFINE_INLINE(AIBridgeReadbackPost) {
    static void Callback(exl::hook::nx64::InlineCtx* ctx) {
        if (!global_config.initialized || !global_config.ai_bridge.active) return;
        AIBridge::readback_hits++;
        const u32 hit = AIBridge::readback_hits;
        const u64 out = ctx->X[19];
        if (!AIBridge::LooksLikePtr(out)) return;
        if (AIBridge::move_records_consumed) AIBridge::ClearMoveWindow();
        const u32 raw_score = AIBridge::ReadU32(out, 0);
        const s32 delta = static_cast<s32>(raw_score);
        const u64 eval_target = AIBridge::PtrHighLow(out, 0xA0);
        const u32 move_id = AIBridge::ReadU32(out, 0xB0);
        const u16 target_species = AIBridge::EvalTargetSpecies(eval_target);
        AIBridge::AddMoveRecord(move_id, target_species, delta);
        if (AIBridge::ShouldLog(hit)) {
            Logging.Log("[ai_bridge] move_eval hit=%u gen=%u move=%u target_species=%u/%s score_delta=%d eval_target=%016lx\n", hit, AIBridge::move_generation, move_id, target_species, AIBridge::KnownSpeciesName(target_species), delta, eval_target);
        }
    }
};

HOOK_DEFINE_INLINE(AIBridgeCandidateScorePost) {
    static void Callback(exl::hook::nx64::InlineCtx* ctx) {
        if (!global_config.initialized || !global_config.ai_bridge.active) return;
        AIBridge::candidate_hits++;
        const u32 hit = AIBridge::candidate_hits;
        const u64 state = ctx->X[19];
        if (!AIBridge::LooksLikePtr(state)) return;
        if (AIBridge::ShouldLog(hit)) {
            Logging.Log("[ai_bridge] candidate_score hit=%u state=%016lx count=%u active=%u/%s fallback=%u/%s records=%u recent=%u gen=%u\n", hit, state, static_cast<u32>(AIBridge::ReadU8(state,0xC5)), AIBridge::CandidateActiveSpecies(state), AIBridge::KnownSpeciesName(AIBridge::CandidateActiveSpecies(state)), AIBridge::CandidateFallbackSwitchSpecies(state), AIBridge::KnownSpeciesName(AIBridge::CandidateFallbackSwitchSpecies(state)), AIBridge::move_record_count, AIBridge::recent_target_count, AIBridge::move_generation);
            AIBridge::DumpCandidates("candidate_score", state, hit);
        }
        AIBridge::ApplySwitchPolicy(state, "candidate_score", hit);
    }
};

void install_ai_bridge_patch() {
    if (AIBridge::installed) return;
#ifdef VERSION_SHIELD
    Logging.Log("[ai_bridge] Shield offsets are not mapped; AI bridge hooks not installed.\n");
#else
    // Hooks are installed before config.toml is parsed; callbacks check global_config.ai_bridge.active at runtime.
    AIBridgeReadbackPost::InstallAtOffset(8211420);
    AIBridgeCandidateScorePost::InstallAtOffset(9342576);
    Logging.Log("[ai_bridge] installed v26 readback_post at 0x7d4bdc and candidate_score_post at 0x8e8e70\n");
#endif
    AIBridge::installed = true;
}
