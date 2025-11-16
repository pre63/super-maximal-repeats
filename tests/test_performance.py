import glob
import os
import time

import pytest

import supermaxrep


@pytest.mark.performance
def test_performance_multi_docs():
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
  repeats = supermaxrep.find_supermaximal_repeats_docs(docs, min_len=20, min_occ=3)
  end_time = time.time()

  duration = end_time - start_time
  print(f"Performance metrics:")
  print(f" - Time taken: {duration:.2f} seconds")
  print(f" - Repeats found: {len(repeats)}")
  print(f" - Throughput: {total_length / duration:.2f} characters/second")
  print(f" - Average miliseconds per document: {(duration / len(docs)) * 1000:.2f} ms/doc")
  print(f" - Average documents per second: {len(docs) / duration:.2f} docs/second")

  # No hard assert on time (machine-dependent), but ensure it runs without error
  assert len(repeats) >= 0
