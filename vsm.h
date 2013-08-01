#pragma once

class WeiboCluster
{
public:
	typedef std::shared_ptr<Weibo>    Document;
	typedef std::vector<Document>     Collection;
	typedef std::vector<Collection>   Cluster;
	typedef std::pair<std::string, std::tuple<size_t, size_t> > Term;
	typedef std::map<std::string, std::tuple<size_t, size_t> > TermMap; //token, (0:index, 1:count)

	void DocumentCluster(Cluster& cluster, Collection& collection, size_t k = 10);

	void Save(const std::string& file);

protected:
	void   VectorSpaceModel(Collection& collection);
	float TermFrequency(const Document& doc, const std::string& term);
	float InverseDocumentFrequency(const Collection& collection, const std::string& term);

	float CosineSimilarity(const std::vector<std::pair<size_t, float>>& x, const std::vector<std::pair<size_t, float>>& y);
	float DotProduct(const std::vector<std::pair<size_t, float>>& x, const std::vector<std::pair<size_t, float>>& y);

	void   CalculateMeanPoints(Cluster& cluster);
	int    FindClosestClusterCenter(const Cluster& cluster, const Document& doc);
	bool   CheckStoppingCriteria(Cluster& prev_cluster, Cluster& new_cluster);

private:
	TermMap terms_; //token, index, count
	std::vector<std::string> topics_;
	std::vector<std::string> mentions_;
};