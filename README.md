# Super Maximal Repeats

A Python library for computing super-maximal repeats in a string or collection of documents, implemented with C++ under the hood using pybind11. Based on the enhanced suffix array algorithm for linear-time computation (in practice, O(n log² n) due to sorting in the suffix array construction).

Super-maximal repeats are maximal repeats that are not substrings of any longer maximal repeat. This is useful for tasks like detecting machine-generated text, as described in the [paper "Unsupervised and Distributional Detection of Machine-Generated Text" by Mathias Gallé et al](https://arxiv.org/abs/2111.02878).

## Installation
```
make install
```

## Usage

```python
import smr

# Compute super-maximal repeats for a single string (character-level by default)
repeats = smr.find_supermaximal_repeats("your_example_string_here", min_len=20, min_occ=3)

# Each repeat is a Repeat object with 'doc_idx' (always 0 for single string), 'start' (starting position), 'len' (length), and 'text' (the repeat substring)
for r in repeats:
    print(f"Repeat: {r.text} (doc_idx: {r.doc_idx}, start: {r.start}, len: {r.len})")

# Compute super-maximal repeats across multiple documents
docs = ["your_first_document_here", "your_second_document_here"]
repeats_docs = smr.find_supermaximal_repeats_docs(docs, min_len=20, min_occ=3, mode="char")

for r in repeats_docs:
    print(f"Repeat: {r.text} (doc_idx: {r.doc_idx}, start: {r.start}, len: {r.len})")
```

- `min_len`: Minimum length of repeats (default: 1).
- `min_occ`: Minimum number of occurrences (default: 2).
- `mode` (for documents only): "char" for character-level repeats (default), or "word" for word-level repeats (splits on whitespace, positions and lengths are in terms of words, text is space-joined).

## Notes

- For large strings (n > 10^6), performance may degrade due to the O(n log² n) suffix array construction, but it is efficient for typical text sizes.
- Outputs a list of `Repeat` objects, each representing a unique super-maximal repeat with one example occurrence (document index, starting position, length, and text).