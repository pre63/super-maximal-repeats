import glob
import os
import random

from datasets import load_dataset

dir_path = "tests/documents"
os.makedirs(dir_path, exist_ok=True)

# Skip if already downloaded (check for approx 2000 files, assuming ~1000 samples in dataset)
if len(glob.glob(os.path.join(dir_path, "*.md"))) >= 2000:
  print("Documents already downloaded. Skipping.")
else:
  ds = load_dataset("gsingh1-py/train", split="train")
  sample_indices = random.sample(range(len(ds)), min(1000, len(ds)))
  for idx in sample_indices:
    human_text = ds[idx]["Human_story"]
    ai_text = ds[idx]["GPT_4-o"]
    if not human_text or not ai_text:
      continue
    with open(os.path.join(dir_path, f"human_{idx}.md"), "w", encoding="utf-8") as f:
      f.write(human_text)
    with open(os.path.join(dir_path, f"ai_{idx}.md"), "w", encoding="utf-8") as f:
      f.write(ai_text)
  print(f"Downloaded {len(sample_indices) * 2} documents.")
