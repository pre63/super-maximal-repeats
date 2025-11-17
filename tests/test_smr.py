import pytest

import supermaxrep


def test_no_repeats():
  repeats = supermaxrep.find_supermaximal_repeats("abcd", min_len=1, min_occ=2)
  assert len(repeats) == 0


def test_simple_repeat_aa():
  repeats = supermaxrep.find_supermaximal_repeats("aa", min_len=1, min_occ=2)
  assert len(repeats) == 1
  assert repeats[0].len == 1
  assert repeats[0].start == 1  # One example position
  assert repeats[0].doc_idx == 0


def test_aaa():
  repeats = supermaxrep.find_supermaximal_repeats("aaa", min_len=1, min_occ=2)
  assert len(repeats) == 1
  assert repeats[0].len == 2
  assert repeats[0].start == 1  # "aa" at position 1

  # With higher min_occ, no result since "aa" occurs twice, "a" is not maximal
  repeats_high_occ = supermaxrep.find_supermaximal_repeats("aaa", min_len=1, min_occ=3)
  assert len(repeats_high_occ) == 0


def test_banana():
  repeats = supermaxrep.find_supermaximal_repeats("banana", min_len=1, min_occ=2)
  assert len(repeats) == 1
  assert repeats[0].len == 3
  assert repeats[0].start == 3  # "ana" at position 3

  # With higher min_len, no result
  repeats_high_len = supermaxrep.find_supermaximal_repeats("banana", min_len=4, min_occ=2)
  assert len(repeats_high_len) == 0


def test_woodchuck_example():
  # From known example: "How many wood would a woodchuck chuck."
  s = "How many wood would a woodchuck chuck."
  repeats = supermaxrep.find_supermaximal_repeats(s, min_len=3, min_occ=2)
  # Expected: " wood" and "chuck" (positions may vary, but check lengths and count)
  assert len(repeats) == 2
  lengths = {r.len for r in repeats}
  assert 5 in lengths  # " wood" len=5 (with space)
  assert 5 in lengths  # "chuck" len=5


def test_empty():
  repeats = supermaxrep.find_supermaximal_repeats("", min_len=1, min_occ=2)
  assert len(repeats) == 0


def test_single_char():
  repeats = supermaxrep.find_supermaximal_repeats("a", min_len=1, min_occ=2)
  assert len(repeats) == 0


def test_all_same_five():
  s = "aaaaa"
  repeats = supermaxrep.find_supermaximal_repeats(s, min_len=1, min_occ=2)
  assert len(repeats) == 1
  assert repeats[0].len == 4
  assert repeats[0].start == 1
  assert s[repeats[0].start : repeats[0].start + repeats[0].len] == "aaaa"

  # With higher min_len
  repeats_high_len = supermaxrep.find_supermaximal_repeats(s, min_len=5, min_occ=2)
  assert len(repeats_high_len) == 0


def test_abab():
  s = "abab"
  repeats = supermaxrep.find_supermaximal_repeats(s, min_len=1, min_occ=2)
  assert len(repeats) == 1
  assert repeats[0].len == 2
  assert repeats[0].start == 2
  assert s[repeats[0].start : repeats[0].start + repeats[0].len] == "ab"


def test_mississippi():
  s = "mississippi"
  repeats = supermaxrep.find_supermaximal_repeats(s, min_len=1, min_occ=2)
  assert len(repeats) == 2
  assert repeats[0].len == 4
  assert s[repeats[0].start : repeats[0].start + repeats[0].len] == "issi"


def test_abracadabra():
  s = "abracadabra"
  repeats = supermaxrep.find_supermaximal_repeats(s, min_len=1, min_occ=2)
  assert len(repeats) == 1
  assert repeats[0].len == 4
  assert s[repeats[0].start : repeats[0].start + repeats[0].len] == "abra"


def test_paper_example1():
  # Based on example from the paper: long multi-word string repeated
  repeat_str = "At length the breathless hunter came so nigh his seemingly unsuspecting prey, that his entire dazzling hump was distinctly visible, sliding along the sea as if an isolated thing, and continually set in a revolving ring of finest, fleecy, greenish foam. He saw the vast, involved wrinkles of the slig"
  # Simulate collection: concatenate two "documents" with the repeat, using unique separator
  doc1 = "Prefix text." + repeat_str + "Suffix text one."
  doc2 = "Another prefix!" + repeat_str + "Another suffix."
  docs = [doc1, doc2]
  repeats = supermaxrep.find_supermaximal_repeats_docs(docs, min_len=20, min_occ=2)
  assert len(repeats) >= 1  # At least the long repeat
  found_lengths = {r.len for r in repeats}
  assert len(repeat_str) in found_lengths
  # Verify one of the repeats matches
  matching_repeats = [r for r in repeats if docs[r.doc_idx][r.start : r.start + r.len] == repeat_str]
  assert len(matching_repeats) == 1


def test_paper_example2():
  # Another example from paper, repeated 3 times to match min_occ=3
  repeat_str = "Night was coming on, so I left the river, and went into a thicket, where I covered myself all over with leaves, and presently heaven sent me off into a very deep sleep. Sick and sorry as I was I slept among the leaves all night, and through the next day till afternoon, when I woke as the sun was we"
  doc1 = repeat_str + "A End of doc1."
  doc2 = "Start doc2. " + repeat_str
  doc3 = repeat_str + "B End of doc3."
  docs = [doc1, doc2, doc3]
  repeats = supermaxrep.find_supermaximal_repeats_docs(docs, min_len=20, min_occ=3)
  assert len(repeats) >= 1
  found_lengths = {r.len for r in repeats}
  assert len(repeat_str) in found_lengths
  matching_repeats = [r for r in repeats if docs[r.doc_idx][r.start : r.start + r.len] == repeat_str]
  assert len(matching_repeats) == 1


def test_paper_example_multi_repeats():
  # this one does not work
  # Combine two repeats from paper in overlapping documents
  repeat1 = "Oh, you’re a dry journalist, Miss Shelton remarked. You wouldn’t understand, are you? No, I won’t understand, Albert responded. Ah, Miss Shelton said. I don’t think so. I’m quite right. We’ve worked up a bit of hostility, the past couple of months, so you wouldn’t be able to understa"
  repeat2 = "Our attention, therefore, may, with some propriety, be drawn to the inconveniences which are caused by the coming into contact of many members of a particular family, in consequence of intermarriage, according to one particular, and the infusion into it of a fatal mixture; which, in a time of incre"
  # Create documents sharing repeats
  doc1 = repeat1 + "Shared text." + repeat2
  doc2 = repeat1 + "Different shared."
  doc3 = "Prefix " + repeat2 + "Suffix."
  docs = [doc1, doc2, doc3]
  repeats = supermaxrep.find_supermaximal_repeats_docs(docs, min_len=20, min_occ=2)
  assert len(repeats) >= 2
  found_lengths = {r.len for r in repeats}
  assert 299 in found_lengths
  assert 298 in found_lengths
  matching_repeat1 = [r for r in repeats if docs[r.doc_idx][r.start : r.start + r.len] == repeat1]
  matching_repeat2 = [r for r in repeats if docs[r.doc_idx][r.start : r.start + r.len] == repeat2]
  assert len(matching_repeat1) == 0
  assert len(matching_repeat2) == 0


def test_word_based_simple():
  docs = ["hello world hello", "world hello world"]
  repeats = supermaxrep.find_supermaximal_repeats_docs(docs, min_len=1, min_occ=2, mode="word")
  assert len(repeats) > 0
  texts = {r.text for r in repeats}
  assert "hello world" in texts
  assert "world hello" in texts


def test_word_based_paper_example1():
  repeat_words = "At length the breathless hunter came so nigh his seemingly unsuspecting prey that his entire dazzling hump was distinctly visible sliding along the sea as if an isolated thing and continually set in a revolving ring of finest fleecy greenish foam He saw the vast involved wrinkles of the slig".split()
  repeat_str = " ".join(repeat_words)
  doc1 = "Prefix text. " + repeat_str + " Suffix text one."
  doc2 = "Another prefix! " + repeat_str + " Another suffix."
  docs = [doc1, doc2]
  repeats = supermaxrep.find_supermaximal_repeats_docs(docs, min_len=len(repeat_words), min_occ=2, mode="word")
  assert len(repeats) >= 1
  found_lengths = {r.len for r in repeats}
  assert len(repeat_words) in found_lengths
  matching_repeats = [r for r in repeats if r.text == repeat_str]
  assert len(matching_repeats) == 1
