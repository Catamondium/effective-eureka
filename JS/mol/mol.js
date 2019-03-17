#!/usr/bin/env node

const COEFFRE = /^(\d+)/g; // beginning coefficient
const TOKRE = /\(.*?\)|([A-Z][a-z]*)(\d*)/g; // groups: Elem, [coeff]
const SUBRE = /\((.*)\)(\d*)/g; // groups: expr, [coeff]

function makeCoeff(c) {
    if (c == '' || c === undefined) {
        return 1;
    } else {
        return parseInt(c);
    }
}

function mass(thing) {
    var acc = 0;
    var big = COEFFRE.exec(thing);
    var bigCoeff = 1;
    if (big !== null) {
        console.log(`Big:\t${big[1]}`);
        bigCoeff = makeCoeff(big[1]);
    }

    while ((m = TOKRE.exec(test)) !== null) {
        let toks = m.slice(1, 3);
        console.log(`token:\t${toks}`);
        acc += ptable[toks[0]] * makeCoeff(toks[1]);
    }

    while ((m = SUBRE.exec(test)) !== null) {
        let toks = m.slice(1, 3);
        console.log(`subexpression:\t${toks}`);

        // Causing infinite looping
        //acc += mass(toks[0]) * makeCoeff(toks[1]);
    }

    return bigCoeff * acc;
}

function translate(source, froms, tos) {
    var table = {};
    var ret = [];
    if (froms.length != tos.length) {
        throw "'froms' & 'tos' must be equal length"
    }

    // trans = str.maketrans ...
    for (const i in froms) {
        table[froms[i]] = tos[i];
    }

    // str.translate ...
    for (const c of source) {
        if (table[c] == undefined) {
            ret.push(c);
        } else {
            ret.push(table[c]);
        }
    }

    return ret.join('');
}

function sanitize(thing) {
    thing = translate(thing, "{[]}", "(())");
    var arr = thing.split('');
    arr = arr.filter(c => c.match(/[a-z0-9\(\)]/i));
    return arr.join('');
}

ptable = {
    H: 1,
    C: 12,
    undefined: 0 // undefined is keyable?
}

// if(__name__ == '__main__') eqiv, supporting web embedding
if (typeof require != 'undefined' && require.main == module) {
    test = "CH3(CH3)2CH3";
    console.log(mass(sanitize(test)));
    console.log(sanitize("C224[](){} \t<>?££$£"));
}
