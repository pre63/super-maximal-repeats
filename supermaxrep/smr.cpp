#include <algorithm>
#include <functional>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace std;

namespace py = pybind11;

struct Suffix {
  int str_idx;
  int pos;
};

struct Repeat {
  int doc_idx;
  int start;
  int len;
  string text;
};

vector<Suffix> buildGSA(const vector<vector<int>> &doc_ids) {
  size_t N = 0;
  for (const auto &di : doc_ids)
    N += di.size();
  vector<Suffix> sa(N);
  size_t idx = 0;
  for (int si = 0; si < doc_ids.size(); ++si) {
    for (int p = 0; p < doc_ids[si].size(); ++p) {
      sa[idx++] = {si, p};
    }
  }
  auto cmp = [&](const Suffix &a, const Suffix &b) -> bool {
    size_t offset = 0;
    while (true) {
      size_t pa = a.pos + offset, pb = b.pos + offset;
      if (pa >= doc_ids[a.str_idx].size() && pb >= doc_ids[b.str_idx].size())
        return false;
      if (pa >= doc_ids[a.str_idx].size())
        return true;
      if (pb >= doc_ids[b.str_idx].size())
        return false;
      int wa = doc_ids[a.str_idx][pa];
      int wb = doc_ids[b.str_idx][pb];
      if (wa != wb)
        return wa < wb;
      ++offset;
    }
  };
  sort(sa.begin(), sa.end(), cmp);
  return sa;
}

vector<int> buildGLCP(const vector<vector<int>> &doc_ids,
                      const vector<Suffix> &sa) {
  size_t N = sa.size();
  vector<vector<int>> rank(doc_ids.size());
  for (int si = 0; si < doc_ids.size(); ++si) {
    rank[si].resize(doc_ids[si].size());
  }
  for (size_t i = 0; i < N; ++i) {
    rank[sa[i].str_idx][sa[i].pos] = i;
  }
  vector<int> lcp(N, 0);
  int h = 0;
  for (int si = 0; si < doc_ids.size(); ++si) {
    for (int p = 0; p < doc_ids[si].size(); ++p) {
      int r = rank[si][p];
      if (r == 0)
        continue;
      Suffix prev = sa[r - 1];
      h = max(0, h - 1);
      size_t pa = p + h, pb = prev.pos + h;
      while (pa < doc_ids[si].size() && pb < doc_ids[prev.str_idx].size() &&
             doc_ids[si][pa] == doc_ids[prev.str_idx][pb]) {
        ++h;
        ++pa;
        ++pb;
      }
      lcp[r] = h;
    }
  }
  return lcp;
}

vector<Repeat> find_smr_docs(const vector<string> &docs, int min_len,
                             int min_occ, string mode = "char") {
  if (docs.empty())
    return {};
  if (mode == "char") {
    vector<vector<int>> doc_ids(docs.size());
    int alph = 0;
    unordered_map<char, int> vocab; // For chars
    for (const auto &d : docs) {
      for (char c : d) {
        if (vocab.find(c) == vocab.end())
          vocab[c] = alph++;
      }
    }
    for (size_t i = 0; i < docs.size(); ++i) {
      doc_ids[i].reserve(docs[i].size());
      for (char c : docs[i])
        doc_ids[i].push_back(vocab[c]);
    }
    size_t N = 0;
    for (const auto &di : doc_ids)
      N += di.size();
    if (N == 0)
      return {};
    auto sa = buildGSA(doc_ids);
    auto lcp = buildGLCP(doc_ids, sa);
    vector<int> bwt(N);
    for (size_t i = 0; i < N; ++i) {
      Suffix suf = sa[i];
      if (suf.pos == 0) {
        bwt[i] = -(suf.str_idx + 1);
      } else {
        bwt[i] = doc_ids[suf.str_idx][suf.pos - 1];
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
          unordered_set<int> seen;
          bool distinct = true;
          for (size_t k = left; k <= right; ++k) {
            int c = bwt[k];
            if (seen.count(c)) {
              distinct = false;
              break;
            }
            seen.insert(c);
          }
          if (distinct) {
            Suffix example = sa[left];
            string text = docs[example.str_idx].substr(example.pos, l);
            Repeat r{example.str_idx, example.pos, l, text};
            res.push_back(r);
          }
        }
      }
    }
    return res;
  } else if (mode == "word") {
    vector<vector<string>> docs_words(docs.size());
    for (size_t i = 0; i < docs.size(); ++i) {
      stringstream ss(docs[i]);
      string word;
      while (ss >> word) {
        docs_words[i].push_back(word);
      }
    }
    bool all_empty = true;
    for (const auto &dw : docs_words) {
      if (!dw.empty()) {
        all_empty = false;
        break;
      }
    }
    if (all_empty)
      return {};
    unordered_map<string, int> vocab;
    int alph = 0;
    for (const auto &dw : docs_words) {
      for (const auto &w : dw) {
        if (vocab.find(w) == vocab.end()) {
          vocab[w] = alph++;
        }
      }
    }
    vector<vector<int>> doc_ids(docs_words.size());
    for (size_t i = 0; i < docs_words.size(); ++i) {
      doc_ids[i].reserve(docs_words[i].size());
      for (const auto &w : docs_words[i]) {
        doc_ids[i].push_back(vocab[w]);
      }
    }
    size_t N = 0;
    for (const auto &di : doc_ids)
      N += di.size();
    if (N == 0)
      return {};
    auto sa = buildGSA(doc_ids);
    auto lcp = buildGLCP(doc_ids, sa);
    vector<int> bwt(N);
    for (size_t i = 0; i < N; ++i) {
      Suffix suf = sa[i];
      if (suf.pos == 0) {
        bwt[i] = -(suf.str_idx + 1);
      } else {
        bwt[i] = doc_ids[suf.str_idx][suf.pos - 1];
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
          unordered_set<int> seen;
          bool distinct = true;
          for (size_t k = left; k <= right; ++k) {
            int c = bwt[k];
            if (seen.count(c)) {
              distinct = false;
              break;
            }
            seen.insert(c);
          }
          if (distinct) {
            Suffix example = sa[left];
            stringstream ss;
            for (int j = 0; j < l; ++j) {
              if (j > 0)
                ss << " ";
              ss << docs_words[example.str_idx][example.pos + j];
            }
            string text = ss.str();
            Repeat r{example.str_idx, example.pos, l, text};
            res.push_back(r);
          }
        }
      }
    }
    return res;
  } else {
    throw runtime_error("Invalid mode: must be 'char' or 'word'");
  }
}

vector<Repeat> find_smr(const string &s, int min_len, int min_occ) {
  vector<string> docs = {s};
  return find_smr_docs(docs, min_len, min_occ);
}

PYBIND11_MODULE(smr, m) {
  py::class_<Repeat>(m, "Repeat")
      .def(py::init<>())
      .def_readwrite("doc_idx", &Repeat::doc_idx)
      .def_readwrite("start", &Repeat::start)
      .def_readwrite("len", &Repeat::len)
      .def_readwrite("text", &Repeat::text);
  m.def("find_supermaximal_repeats", &find_smr, py::arg("s"),
        py::arg("min_len") = 1, py::arg("min_occ") = 2);
  m.def("find_supermaximal_repeats_docs", &find_smr_docs, py::arg("docs"),
        py::arg("min_len") = 1, py::arg("min_occ") = 2,
        py::arg("mode") = "char");
}