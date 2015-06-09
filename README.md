# Introduction

The repository is the codes for __RST-Invariant Sketch Retrieval Based On Circular Description__.

# Installation

## Install Julia

Just follow the instructions in [julialang.org](http://julialang.org/downloads/). _Julia + Juno IDE bundles_ is recommended for people who hates command lines.

For comrades that live inside the great firewall, type `git config --global url."https://".insteadOf git://` in command line before further operations.

# Usage

# Performance

## Natural Images

Kendall's correlation coefficient of [TVCG-Benchmark](http://cybertron.cg.tu-berlin.de/eitz/tvcg_benchmark/index.html):

![Kendall](https://cloud.githubusercontent.com/assets/853842/8049593/c11cee78-0e93-11e5-861f-a4e57c9c2b9e.png)

## Product Images

Average precision on [PI-100](http://research.microsoft.com/en-us/people/xingx/pi100.aspx) dataset.

![Precision](https://cloud.githubusercontent.com/assets/853842/8049592/c11c1d72-0e93-11e5-8134-2b84fb633c8f.png)

The images are rotated in random angles:

![Precision with Rotation](https://cloud.githubusercontent.com/assets/853842/8049594/c11f6e64-0e93-11e5-85bb-7ae2916d4bfd.png)