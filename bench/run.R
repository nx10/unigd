#!/usr/bin/env Rscript
# Run unigd benchmarks and save results for the vignette.
#
# Usage: Rscript bench/run.R

source(file.path("bench", "benchmark.R"))

message("Saving example plot thumbnails...")
save_example_plots("vignettes")

message("Running unigd benchmarks...")
results <- run_benchmarks()
message("Done. ", nrow(results), " measurements collected.")

out_path <- file.path("vignettes", "bench_results.rds")
saveRDS(results, out_path)
message("Results saved to ", out_path)

message("Rendering benchmark charts...")
save_benchmark_charts(results, "vignettes")
message("All done.")
