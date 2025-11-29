import glob
import os
import time

import pytest

import supermaxrep


@pytest.mark.performance
def test_performance_multi_docs_char():
  doc_dir = "tests/documents"
  doc_files = glob.glob(os.path.join(doc_dir, "*.md"))
  assert len(doc_files) > 0, "No documents found; run 'make download' first."

  docs = []
  total_length = 0
  for f in doc_files:
    with open(f, "r", encoding="utf-8") as ff:
      text = ff.read()
      docs.append(text)
      total_length += len(text)

  print(f"Loaded {len(docs)} documents with total length {total_length} characters.")

  start_time = time.time()
  repeats = supermaxrep.find_supermaximal_repeats_docs(docs, min_len=20, min_occ=3, mode="char")
  end_time = time.time()

  duration = end_time - start_time
  print(f"Character-based performance metrics:")
  print(f" - Time taken: {duration:.2f} seconds")
  print(f" - Repeats found: {len(repeats)}")
  print(f" - Throughput: {total_length / duration:.2f} characters/second")
  print(f" - Average miliseconds per document: {(duration / len(docs)) * 1000:.2f} ms/doc")
  print(f" - Average documents per second: {len(docs) / duration:.2f} docs/second")

  assert len(repeats) >= 0


@pytest.mark.performance
def test_performance_multi_docs_word():
  doc_dir = "tests/documents"
  doc_files = glob.glob(os.path.join(doc_dir, "*.md"))
  assert len(doc_files) > 0, "No documents found; run 'make download' first."

  docs = []
  total_words = 0
  for f in doc_files:
    with open(f, "r", encoding="utf-8") as ff:
      text = ff.read()
      docs.append(text)
      total_words += len(text.split())

  print(f"Loaded {len(docs)} documents with total {total_words} words (approx).")

  start_time = time.time()
  repeats = supermaxrep.find_supermaximal_repeats_docs(docs, min_len=3, min_occ=2, mode="word")
  end_time = time.time()

  duration = end_time - start_time
  print(f"Word-based performance metrics:")
  print(f" - Time taken: {duration:.2f} seconds")
  print(f" - Repeats found: {len(repeats)}")
  print(f" - Throughput: {total_words / duration:.2f} words/second (approx)")
  print(f" - Average miliseconds per document: {(duration / len(docs)) * 1000:.2f} ms/doc")
  print(f" - Average documents per second: {len(docs) / duration:.2f} docs/second")

  assert len(repeats) >= 0
