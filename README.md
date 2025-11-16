# Super Maximal Repeats

A Python library for computing super-maximal repeats in a string, implemented with C++ under the hood using pybind11. Based on the enhanced suffix array algorithm for linear-time computation (in practice, O(n log² n) due to sorting in the suffix array construction).

Super-maximal repeats are maximal repeats that are not substrings of any longer maximal repeat. This is useful for tasks like detecting machine-generated text, as described in the [paper "Unsupervised and Distributional Detection of Machine-Generated Text" by Mathias Gallé et al](https://arxiv.org/abs/2111.02878).

## Installation
   ```
   make install
   ```

## Usage

```python
import supermaxrep

# Compute super-maximal repeats
repeats = supermaxrep.find_supermaximal_repeats("your_example_string_here", min_len=20, min_occ=3)

# Each repeat is a Repeat object with 'start' (starting index in the string) and 'len' (length)
for r in repeats:
    substring = "your_example_string_here"[r.start : r.start + r.len]
    print(f"Repeat: {substring} (start: {r.start}, len: {r.len})")
```

- `min_len`: Minimum length of repeats (default: 1).
- `min_occ`: Minimum number of occurrences (default: 2).
- For a collection of documents (as in the paper), concatenate them with unique separators not present in the text (e.g., `\x01`, `\x02`, etc.) to allow cross-document repeats without artificial spanning.

## Notes

- The input string should not contain the null character (`\0`), as it is used internally as a sentinel.
- For large strings (n > 10^6), performance may degrade due to the O(n log² n) suffix array construction, but it is efficient for typical text sizes.
- Outputs a list of `Repeat` objects, each representing a unique super-maximal repeat with one example starting position.
