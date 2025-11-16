
#include <algorithm>
#include <functional>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <string>
#include <unordered_set>
#include <vector>

using namespace std;

namespace py = pybind11;

struct Suffix {
  int str_idx;
  int pos;
};

vector<Suffix> buildGSA(const vector<string> &docs) {
  size_t N = 0;
  for (const auto &d : docs)
    N += d.size();
  vector<Suffix> sa(N);
  size_t idx = 0;
  for (int si = 0; si < docs.size(); ++si) {
    for (int p = 0; p < docs[si].size(); ++p) {
      sa[idx++] = {si, p};
    }
  }
  auto cmp = [&](const Suffix &a, const Suffix &b) -> bool {
    int pa = a.pos, pb = b.pos;
    const string &sa_str = docs[a.str_idx];
    const string &sb_str = docs[b.str_idx];
    while (pa < sa_str.size() && pb < sb_str.size()) {
      if (sa_str[pa] != sb_str[pb])
        return sa_str[pa] < sb_str[pb];
      ++pa;
      ++pb;
    }
    char a_char = (pa == sa_str.size()) ? static_cast<char>(-(a.str_idx + 1))
                                        : sa_str[pa];
    char b_char = (pb == sb_str.size()) ? static_cast<char>(-(b.str_idx + 1))
                                        : sb_str[pb];
    return a_char < b_char;
  };
  sort(sa.begin(), sa.end(), cmp);
  return sa;
}

vector<int> buildGLCP(const vector<string> &docs, const vector<Suffix> &sa) {
  size_t N = sa.size();
  vector<vector<int>> rank(docs.size());
  for (int si = 0; si < docs.size(); ++si) {
    rank[si].resize(docs[si].size());
  }
  for (size_t i = 0; i < N; ++i) {
    rank[sa[i].str_idx][sa[i].pos] = i;
  }
  vector<int> lcp(N, 0);
  int h = 0;
  for (int si = 0; si < docs.size(); ++si) {
    for (int p = 0; p < docs[si].size(); ++p) {
      int r = rank[si][p];
      if (r == 0)
        continue;
      Suffix prev = sa[r - 1];
      h = max(0, h - 1);
      int pa = p + h, pb = prev.pos + h;
      while (pa < docs[si].size() && pb < docs[prev.str_idx].size() &&
             docs[si][pa] == docs[prev.str_idx][pb]) {
        ++h;
        ++pa;
        ++pb;
      }
      lcp[r] = h;
    }
  }
  return lcp;
}

struct Repeat {
  int doc_idx;
  int start;
  int len;
};

vector<Repeat> find_smr_docs(const vector<string> &docs, int min_len,
                             int min_occ) {
  if (docs.empty())
    return {};
  size_t N = 0;
  for (const auto &d : docs)
    N += d.size();
  if (N == 0)
    return {};
  auto sa = buildGSA(docs);
  auto lcp = buildGLCP(docs, sa);
  vector<char> bwt(N);
  for (size_t i = 0; i < N; ++i) {
    Suffix suf = sa[i];
    if (suf.pos == 0) {
      bwt[i] = static_cast<char>(-(suf.str_idx + 1));
    } else {
      bwt[i] = docs[suf.str_idx][suf.pos - 1];
    }
  }
  vector<Repeat> res;
  size_t i = 1;
  while (i < N) {
    if (lcp[i] == 0) {
      ++i;
      continue;
    }
    int l = lcp[i];
    size_t start = i;
    while (i < N && lcp[i] == l) {
      ++i;
    }
    size_t end = i - 1;
    bool is_local_max = (start == 1 || lcp[start - 1] < l) &&
                        (end + 1 >= N || lcp[end + 1] < l);
    if (is_local_max) {
      size_t left = start - 1;
      size_t right = end;
      int occ = right - left + 1;
      if (occ >= min_occ && l >= min_len) {
        unordered_set<char> seen;
        bool distinct = true;
        for (size_t k = left; k <= right; ++k) {
          char c = bwt[k];
          if (seen.count(c)) {
            distinct = false;
            break;
          }
          seen.insert(c);
        }
        if (distinct) {
          Suffix example = sa[left];
          Repeat r{example.str_idx, example.pos, l};
          res.push_back(r);
        }
      }
    }
  }
  return res;
}

vector<Repeat> find_smr(const string &s, int min_len, int min_occ) {
  vector<string> docs = {s};
  auto repeats = find_smr_docs(docs, min_len, min_occ);
  return repeats;
}

PYBIND11_MODULE(smr, m) {
  py::class_<Repeat>(m, "Repeat")
      .def(py::init<>())
      .def_readwrite("doc_idx", &Repeat::doc_idx)
      .def_readwrite("start", &Repeat::start)
      .def_readwrite("len", &Repeat::len);
  m.def("find_supermaximal_repeats", &find_smr, py::arg("s"),
        py::arg("min_len") = 1, py::arg("min_occ") = 2);
  m.def("find_supermaximal_repeats_docs", &find_smr_docs, py::arg("docs"),
        py::arg("min_len") = 1, py::arg("min_occ") = 2);
}