// Copyright (C) Amaya Software Ltd. All rights reserved.

#include "genJavaScript.h"

void CGJSjavascriptmain::createJavaScriptFiles(void)
{
	for(const auto& docp : mainObj->documents)
	{
		vector<string> txts;
		for(auto& elem : docp.second.getObjects())
		{
			switch(elem->getObjType())
			{
			case objTypes::gTypeText:
				{
					if((!elem->getName().length()) || (elem->getName() == "genJavaScript")) 
						txts.push_back(elem->getTxt());
					break;
				}
			default:
				break;
			};
		}
		for(auto& elem : docp.second.getObjects())
		{
			switch(elem->getObjType())
			{
			case objTypes::gTypeStruct:
				{
					createJavaScriptClass(docp.first, elem.get(), txts);
					break;
				}
			default:
				break;
			};
		}
	}

}

void CGJSjavascriptmain::createJavaScriptClass(const string& nameDoc, const CGbaseobj* p, const vector<string>& txts)
{
	string output;
	CGfile cfile;

	const CGstruct* cls = dynamic_cast<const CGstruct*>(p);
	if(!cls)
	{
		string err;
		err.append("Wrong dynamic cast to class, file-'").append(nameDoc).append("' object-'").append(p->getName());
		throw exception(err.c_str());
	}
	string name = cls->getName();

	if(mainObj->outdir.length())
	{
		output.assign(mainObj->outdir).append("\\");
	}
	else
	{
		auto z = nameDoc.find_last_of("\\");
		if (z == string::npos)
			z = nameDoc.find_last_of("/");
		if (z != string::npos)
			output.assign(nameDoc.substr(0, z+1));
	}
	output.append(name).append(".ts");
	cfile.open(output);
	output.clear();

	output.append(COPYRIGHT);
	cfile.write(output, true);

	clear();
	for(auto& elem : cls->getObjects())
	{
		if(elem->getObjType() == objTypes::gTypeField)
		{
			const CGfield* fld = dynamic_cast<const CGfield*>(elem.get());
			if(!fld)
			{
				string err;
				err.append("Wrong dynamic cast to class, file-'").append(nameDoc).append("' class-'").append(cls->getName()).append("' object-'").append(elem->getName());
				throw exception(err.c_str());
			}

			if(!(fld->getProperty() & CG_JS_IGNORE_ON))
			{
				unique_ptr<CGJSbasecls> bcls;
				bcls = CGJSbasecls::getBaseCls(fld->getFieldType()->getFieldTypeStr());
				fill(fld, bcls.get());
			}
		}
	}

	for(auto& elem : txts)
	{
		output.append(elem);
	}
	for(auto& elem : gImport)
	{
		output.append("import ").append(elem).append("\n");
	}
	{
		auto it = mainObj->iniInfo.find("JAVASCRIPT_IMPORT");
		if(it != end(mainObj->iniInfo))
		{
			for(auto& elem : it->second)
			{
				output.append("import ").append(elem.first);
				if(elem.second.length())
					output.append(" = ").append(elem.second);
				output.append(";\n");
			}
		}
	}

	//output.append("\n");
	cfile.write(output, true);

	for(const auto& obj : cls->getObjects())
	{
		switch(obj->getObjType())
		{
		case objTypes::gTypeText:
			{
				if((!obj->getName().length()) || (obj->getName() == "genJavaScript")) 
					output.append(obj->getTxt());
				break;
			}
		default:
			break;
		};
	}

	//for(auto& elem1 : txts)
	//{
	//	makeTabMargin(output, 1);
	//	output.append(elem1);
	//}
	output.append("\n");
	cfile.write(output, true);



	output.append("export class ").append(cls->getName()).append(" ");	
	if(cls->getJSExtends().length())
		output.append(cls->getJSExtends());	
	output.append("{\n");
	for(auto& elem : gFields)
	{
		for(auto& elem1 : elem)
		{
			makeTabMargin(output, 1);
			output.append(elem1);
		}
	}
	output.append("\n");
	for(auto& elem : gGetterSetter)
	{
		for(auto& elem1 : elem)
		{
			makeTabMargin(output, 1);
			output.append(elem1);
		}
	}
	//output.append("\n");
	if(cls->getProperty() & CG_CLASS_ARCHIVED_ON)
	{
		makeTabMargin(output, 1);
		output.append("constructor(archived?:boolean) {\n");
	}
	else
	{
		makeTabMargin(output, 1);
		output.append("constructor() {\n");
	}

	if(cls->getProperty() & CG_JS_SUPER_ON)
	{
		if(cls->getProperty() & CG_CLASS_ARCHIVED_ON)
		{
			makeTabMargin(output, 2);
			output.append("super(archived);\n");
		}
		else
		{
			makeTabMargin(output, 2);
			output.append("super();\n");
		}
	}

	if(cls->getProperty() & CG_CLASS_SELFIELD_ON)
	{
		makeTabMargin(output, 2);
		output.append("this._prop2path={\n");
		bool first = true;
		for(auto& elem : cls->getObjects())
		{
			if(elem->getObjType() == objTypes::gTypeField)
			{
				const CGfield* fld = dynamic_cast<const CGfield*>(elem.get());
				if(!fld)
				{
					string err;
					err.append("Wrong dynamic cast to class, file-'").append(nameDoc).append("' class-'").append(cls->getName()).append("' object-'").append(elem->getName());
					throw exception(err.c_str());
				}
				if(!(fld->getProperty() & CG_JS_IGNORE_ON))
				{
					if(first)
						first = false;
					else
						output.append(",\n");
					makeTabMargin(output, 3);
					output.append("'").append(fld->getName()).append("':").append(fld->getFieldIndexString()).append("");
				}
			}
		}
		output.append("\n");

		makeTabMargin(output, 2);
		output.append("}\n");

	}

	makeTabMargin(output, 1);
	output.append("}\n");
	output.append("\n");

	if(cls->getProperty() & CG_CLASS_SELFIELD_ON)
	{

		makeTabMargin(output, 1);
		output.append("getModelType():string{\n");
		if(cls->getProperty() & CG_CLASS_ARCHIVED_ON)
		{
			makeTabMargin(output, 2);
			output.append("if(this._archived)\n");
			makeTabMargin(output, 3);
			output.append("return \"").append(cls->getArcType()).append("\";\n");
			makeTabMargin(output, 2);
			output.append("else\n");
			makeTabMargin(output, 3);
			output.append("return \"").append(cls->getModelType()).append("\";\n");
		}
		else
		{
			makeTabMargin(output, 2);
			output.append("return \"").append(cls->getModelType()).append("\";\n");
		}
		makeTabMargin(output, 1);
		output.append("}\n\n");

		//makeTabMargin(output, 1);
		//output.append("getPropSetPath(props:Array<string>):Array<number[]>{\n");
		//makeTabMargin(output, 2);
		//output.append("if (!props)  return [];\n");
		//makeTabMargin(output, 2);
		//output.append("let path=[], prop2p = this._prop2path;\n");
		//makeTabMargin(output, 2);
		//output.append("props.forEach(p=>{\n");
		//makeTabMargin(output, 3);
		//output.append("let pth = prop2p[p];\n");
		//makeTabMargin(output, 3);
		//output.append("if (pth)\n");
		//makeTabMargin(output, 4);
		//output.append("path.push(pth);\n");
		//makeTabMargin(output, 2);
		//output.append("})\n");
		//makeTabMargin(output, 2);
		//output.append("return path;\n");
		//makeTabMargin(output, 1);
		//output.append("}\n\n");


		makeTabMargin(output, 1);
		output.append("parse(parser:CommMsgParser,propSet?:Array<ResponceSequence>):").append(cls->getName()).append("{\n");
		makeTabMargin(output, 2);
		output.append("let sz;\n");
		makeTabMargin(output, 2);
		output.append("this._init=true;\n");
		makeTabMargin(output, 2);
		output.append("if (propSet && propSet.length >0){//the filter should contain property set to parse\n");
		//makeTabMargin(output, 3);
		//output.append("let param:Parameter = filter.getParameter(FormalParameter.PropSet.code);\n");
		//makeTabMargin(output, 3);
		//output.append("if (!param) throw 'Property set is expected but not specified';\n");
		makeTabMargin(output, 3);
		output.append("let depth = -1;\n");
		makeTabMargin(output, 3);
		output.append("propSet.forEach((rs:ResponceSequence,k:number)=>{\n");
		makeTabMargin(output, 4);
		output.append("if (depth == -1) depth = rs.depth;\n");
		makeTabMargin(output, 4);
		output.append("let propname = rs.propname;\n");
		makeTabMargin(output, 4);
		output.append("if (rs.depth==depth) {//curr object\n");
		makeTabMargin(output, 5);
		output.append("if (rs.proptype){//embedded object\n");
		makeTabMargin(output, 6);
		output.append("if (!this[propname])  //this could happen if we convert primitive to structure\n");
		makeTabMargin(output, 7);
		output.append("this[propname] = ModelFactory.createModelIsntance(rs.proptype);\n");
		makeTabMargin(output, 6);
		output.append("let cmethod = Utils.isCollection(this[propname]),_propSet:Array<ResponceSequence>=this.getResponsePropSet(propSet,depth,k);\n");
		makeTabMargin(output, 6);
		output.append("if (cmethod) {\n");
		makeTabMargin(output, 7);
		output.append("sz = parser.parseUINT32();\n");
		makeTabMargin(output, 7);
		output.append("for (let i=0;i<sz;i++) {\n");
		makeTabMargin(output, 8);
		output.append("this[propname][cmethod](ModelFactory.createModelIsntance(rs.proptype).parse(parser, _propSet));\n");
		makeTabMargin(output, 7);
		output.append("}\n");
		makeTabMargin(output, 6);
		output.append("}\n");
		makeTabMargin(output, 6);
		output.append("else{\n");
		makeTabMargin(output, 7);
		output.append("sz = parser.parseUINT32();\n");
		makeTabMargin(output, 7);
		output.append("if(sz > 0)\n");
		makeTabMargin(output, 8);
		output.append("this[propname] = ModelFactory.createModelIsntance(rs.proptype).parse(parser, _propSet);\n");
		makeTabMargin(output, 7);
		output.append("else\n");
		makeTabMargin(output, 8);
		output.append("this[propname] = undefined;\n");
		makeTabMargin(output, 6);
		output.append("}\n");
		//makeTabMargin(output, 6);
		//output.append("}\n");
		makeTabMargin(output, 5);
		output.append("}\n");
		makeTabMargin(output, 5);
		output.append("else{\n");



		makeTabMargin(output, 6);
		output.append("switch (propname){\n");
		for(auto& elem : gParse)
		{
			for(auto& elem1 : elem)
			{
				makeTabMargin(output, 7);
				output.append(elem1);
			}
			//makeTabMargin(output, 4);
			//output.append("brake;\n");
		}
		makeTabMargin(output, 7);
		output.append("default:\n");







		makeTabMargin(output, 8);
		output.append("break;\n");
		makeTabMargin(output, 6);
		output.append("}\n");
		makeTabMargin(output, 5);
		output.append("}\n");
		makeTabMargin(output, 4);
		output.append("}\n");


		makeTabMargin(output, 3);
		output.append("})\n");
		makeTabMargin(output, 2);
		output.append("}\n");
		makeTabMargin(output, 2);
		output.append("else\n");
		makeTabMargin(output, 2);
		output.append("{\n");
		for(auto& elem : gParseFull)
		{
			for(auto& elem1 : elem)
			{
				makeTabMargin(output, 3);
				output.append(elem1);
			}
		}

		makeTabMargin(output, 2);
		output.append("}\n");
		makeTabMargin(output, 2);
		output.append("this._init=false;\n");
		makeTabMargin(output, 2);
		output.append("return this;\n");
		makeTabMargin(output, 1);
		output.append("}\n\n");
/////////////////

		if((cls->getProperty() & CG_CLASS_UPDATEHANDLER_ON) && (cls->getName() == "CECase"))
		{
			makeTabMargin(output, 1);
			output.append("compose(msgBody?:CommMsgBody):CommMsgBody{\n");
			makeTabMargin(output, 2);
			output.append("let mbdy:CommMsgBody = super.compose(msgBody);\n");
			makeTabMargin(output, 2);
			output.append("mbdy.composeString(this.getModelType());\n");
			if(!gObjectId.size())
			{
				string err;
				err.append("No object ID set in class, file-'").append(nameDoc).append("' class-'").append(cls->getName());
				throw exception(err.c_str());
			}
			makeTabMargin(output, 2);
			output.append(gObjectId);
			if(!gVersionId.size())
			{
				string err;
				err.append("No version ID set in class, file-'").append(nameDoc).append("' class-'").append(cls->getName());
				throw exception(err.c_str());
			}
			makeTabMargin(output, 2);
			output.append(gVersionId);
			makeTabMargin(output, 2);
			output.append("mbdy.composeUINT32(this._dirtyprops.size);\n");
			makeTabMargin(output, 2);
			output.append("this._dirtyprops.forEach((propname)=>{\n");
			makeTabMargin(output, 3);
			output.append("mbdy.composeString(propname);\n");
			makeTabMargin(output, 3);
			output.append("switch (propname) {\n");
			for(auto& elem : gCompose)
			{
				for(auto& elem1 : elem)
				{
					makeTabMargin(output, 4);
					output.append(elem1);
				}
				//makeTabMargin(output, 4);
				//output.append("brake;\n");
			}

			makeTabMargin(output, 4);
			output.append("default:\n");
			makeTabMargin(output, 5);
			output.append("break;\n");
			makeTabMargin(output, 3);
			output.append("}\n");
			makeTabMargin(output, 2);
			output.append("});\n");
			makeTabMargin(output, 2);
			output.append("return mbdy;\n");
			makeTabMargin(output, 1);
			output.append("}\n\n");
		}
	}

	cfile.write(output, true);

	if(gRemoveCont.size())
	{
		makeTabMargin(output, 1);
		output.append("removeItem(which:number,index:number):boolean{\n");
		makeTabMargin(output, 2);
		output.append("let items;\n");
		makeTabMargin(output, 2);
		for(auto& elem : gRemoveCont)
		{
			output.append("if(which==").append(to_string(elem.first)).append(")\n");
			makeTabMargin(output, 3);
			output.append("items = this.").append(elem.second).append(";\n");
			makeTabMargin(output, 2);
			output.append("else ");
		}
		output.append("\n");
		makeTabMargin(output, 3);
		output.append("return false;\n\n");
		makeTabMargin(output, 2);
		output.append("if (index < 0 || items.size == 0) return false;\n");
		makeTabMargin(output, 2);
		output.append("let i=0, k = items.keys(),a,entry;\n");
		makeTabMargin(output, 2);
		output.append("while (true) {\n");
		makeTabMargin(output, 3);
		output.append("entry = k.next();\n");
		makeTabMargin(output, 3);
		output.append("if(i==index){\n");
		makeTabMargin(output, 4);
		output.append("a=entry;\n");
		makeTabMargin(output, 4);
		output.append("break;\n");
		makeTabMargin(output, 3);
		output.append("}\n");
		makeTabMargin(output, 3);
		output.append("if(entry.done)\n");
		makeTabMargin(output, 4);
		output.append("break;\n");
		makeTabMargin(output, 3);
		output.append("i++;\n");
		makeTabMargin(output, 2);
		output.append("}\n");
		makeTabMargin(output, 2);
		output.append("if(a)\n");
		makeTabMargin(output, 3);
		output.append("return items.delete(a.value);\n");
		makeTabMargin(output, 2);
		output.append("else\n");
		makeTabMargin(output, 3);
		output.append("return false;\n");
		makeTabMargin(output, 1);
		output.append("}\n\n");
	}

	//output.append("\n");
	cfile.write(output, true);

	for(auto& elem : gSubscrParse)
	{
		for(auto& elem1 : elem)
		{
			makeTabMargin(output, 1);
			output.append(elem1);
		}
	}

	if(gSubscr.size())
	{
//		output.append("\n");
		if(cls->getProperty() & CG_CLASS_SPECIAL_1)		
		{
			fillSpecial1(cls, cfile);
		}
		else
		{
			cfile.write(output, true);
			makeTabMargin(output, 1);
			output.append("parseNode(node:CommNode|ArrayBuffer,path:number[]):").append(cls->getName()).append("{\n");
			makeTabMargin(output, 2);
			output.append("if(!path) return null;\n");
			makeTabMargin(output, 2);
			output.append("let pl = path.length, leafnode:any, nnum=path[0];\n");

			makeTabMargin(output, 2);
			output.append("if(node instanceof  CommNode)\n");
			makeTabMargin(output, 3);
			output.append("leafnode = node;\n");
			makeTabMargin(output, 2);
			output.append("else if (node instanceof ArrayBuffer)\n");
			makeTabMargin(output, 3);
			output.append("leafnode = node;\n");
			makeTabMargin(output, 2);
			output.append("switch(nnum){\n");
			cfile.write(output, true);

			for(auto& elem : gSubscr)
			{
				for(auto& elem1 : elem)
				{
					makeTabMargin(output, 3);
					output.append(elem1);
				}
				//makeTabMargin(output, 4);
				//output.append("brake;\n");
			}
			cfile.write(output, true);
			makeTabMargin(output, 2);
			output.append("}\n");
			makeTabMargin(output, 2);
			output.append("return this;\n");
			makeTabMargin(output, 1);
			output.append("}\n");
		}
	}

	output.append("}\n");
	cfile.write(output, true);
}

void CGJSjavascriptmain::fill(const CGfield* fld, CGJSbasecls* bcls)
{
	string s;
	vector<string> buf;

	gFields.push_back(bcls->fillFields(fld));
	if(fld->getUpdateHandler().size())
	{
		gGetterSetter.push_back(bcls->fillGetterSetter(fld));
	}
	if (fld->getProperty() & CG_CLASS_OBJECTID_ON)	
	{
		if(gObjectId.size())
		{
			string err;
			err.append("Double object ID, class-'").append(fld->getClassName()).append("' field-'").append(fld->getName());
			throw exception(err.c_str());
		}
		else
		{
			string str = bcls->fillObjectId(fld);
			if(!str.size())
			{
				string err;
				err.append("Field cannot be object ID, class-'").append(fld->getClassName()).append("' field-'").append(fld->getName());
				throw exception(err.c_str());
			}
			gObjectId.append("mbdy.").append(str).append(";\n");
		}
	}
	if (fld->getProperty() & CG_CLASS_VERSION_ON)	
	{
		if(gVersionId.size())
		{
			string err;
			err.append("Double version ID, class-'").append(fld->getClassName()).append("' field-'").append(fld->getName());
			throw exception(err.c_str());
		}
		else
		{
			string str = bcls->fillVersionId(fld);
			if(!str.size())
			{
				string err;
				err.append("Field cannot be version ID, class-'").append(fld->getClassName()).append("' field-'").append(fld->getName());
				throw exception(err.c_str());
			}
			gVersionId.append("mbdy.").append(str).append(";\n");
		}
	}
	if (fld->getProperty() & CG_CLASS_SUBSCR_LEAFNODE)
	{
		gSubscrParse.push_back(bcls->fillSubscrParse(fld, fld->getFieldType(), fld->getName(), mainObj ));
		gSubscr.push_back(bcls->fillSubscr(fld, fld->getFieldType(), fld->getName(), mainObj ));
		if(fld->getFieldType()->isContainer())
		{
			gRemoveCont.insert(make_pair(fld->getLeafNodeIndex(), fld->getName()));
		}
	}

	if(fld->getUpdateHandler().size())
	{
		gCompose.push_back(bcls->fillCompose(fld, fld->getFieldType(), fld->getName(), mainObj ));
	}

	if(!(fld->getProperty() & CG_FIELD_HIDDEN_ON)) 
	{
		if(fld->getProperty() & CG_CLASS_SELFIELD_ON)
		{
			gParse.push_back(bcls->fillParse(fld, fld->getFieldType(), fld->getName(), mainObj ));
		}

	}
	const map<string, tuple<bool,bool,bool>>& channels =  fld->getPcChannels();
	bool dum = false;
	for(auto& elem : channels)
	{
		if(!get<0>(elem.second))
		{
			if(get<2>(elem.second))
			{
				if((elem.first == "default"))
				{
					gParseFull.push_back(bcls->fillParseF(fld, fld->getFieldType(), fld->getName(), mainObj ));
				}
			}
		}
	}
}

void CGJSjavascriptmain::clear(void)
{
	gImport.clear();
	gFields.clear();
	gSubscrParse.clear();
	gSubscr.clear();
	gRemoveCont.clear();
	gParse.clear();
	gParseFull.clear();
	gGetterSetter.clear();
	gObjectId.clear();
	gVersionId.clear();
	gCompose.clear();
}

void CGJSjavascriptmain::fillSpecial1(const CGstruct* cls, CGfile& cfile)
{
	string output;
	const CGfield* fld = nullptr;

	for(auto& elem : cls->getObjects())
	{
		if(elem->getObjType() == objTypes::gTypeField)
		{
			fld = dynamic_cast<const CGfield*>(elem.get());
			if(!fld)
			{
				string err;
				err.append("Wrong dynamic cast to class-'").append(cls->getName()).append("' object-'").append(elem->getName());
				throw exception(err.c_str());
			}
			if(!fld->isArray())
			{
				fld = nullptr;
			}
			else
				break;
		}
	}

	if(!fld) throw exception(string("wrong Special1 attributes in class ").append(cls->getName()).c_str());
	//size_t z = fld->getFieldType()->getArraySize();
	uint32_t idx = fld->getLeafNodeIndex();

	makeTabMargin(output, 1);
	output.append("parseNode(node:CommNode|ArrayBuffer,path:number[]):").append(cls->getName()).append("{\n");
	makeTabMargin(output, 2);
	output.append("if(!path) return null;\n");
	makeTabMargin(output, 2);
	output.append("let pl = path.length, leafnode:any;\n");

	makeTabMargin(output, 2);
	output.append("if(node instanceof  CommNode)\n");
	makeTabMargin(output, 3);
	output.append("leafnode = node;\n");
	makeTabMargin(output, 2);
	output.append("else if (node instanceof ArrayBuffer)\n");
	makeTabMargin(output, 3);
	output.append("leafnode = node;\n");
	makeTabMargin(output, 2);
	output.append("switch(pl){\n");
	cfile.write(output, true);

	makeTabMargin(output, 3);
	output.append("case 1:\n");
	makeTabMargin(output, 4);
	output.append("if(!leafnode.isNode())\n");
	makeTabMargin(output, 5);
	output.append("throw \"NODE_EXPECTED\";\n");
	makeTabMargin(output, 4);
	output.append("let backets = leafnode.findSubnode([0]);\n");
	makeTabMargin(output, 4);
	output.append("if(!backets)\n");
	makeTabMargin(output, 5);
	output.append("throw \"TREND_IS_EMPTY\";\n");
	makeTabMargin(output, 4);
	output.append("for(let i=0,l=backets.getSize();i<l;i++){\n");
	makeTabMargin(output, 5);
	output.append("this._parse_").append(fld->getName()).append("(backets.findSubnode([i]).leaf,i);\n");
	makeTabMargin(output, 4);
	output.append("}\n");
	makeTabMargin(output, 4);
	output.append("this._parse_").append(fld->getName()).append("Pos(leafnode.findSubnode([1]).leaf);\n");
	makeTabMargin(output, 4);
	output.append("this._parse_").append(fld->getName()).append("Full(leafnode.findSubnode([2]).leaf);\n");
	makeTabMargin(output, 4);
	output.append("break;\n");
	makeTabMargin(output, 3);
	output.append("case 2:\n");
	makeTabMargin(output, 4);
	output.append("if (path[1]==1){\n");
	makeTabMargin(output, 5);
	output.append("this._parse_").append(fld->getName()).append("Pos(leafnode);\n");
	makeTabMargin(output, 4);
	output.append("}\n");
	makeTabMargin(output, 4);
	output.append("else if (path[1]==2){\n");
	makeTabMargin(output, 5);
	output.append("this._parse_").append(fld->getName()).append("Full(leafnode);\n");
	makeTabMargin(output, 4);
	output.append("}\n");
	makeTabMargin(output, 4);
	output.append("break;\n");
	makeTabMargin(output, 3);
	output.append("case 3:\n");
	makeTabMargin(output, 4);
	output.append("this._parse_").append(fld->getName()).append("(leafnode,path[2]);\n");
	makeTabMargin(output, 4);
	output.append("break;\n");

	cfile.write(output, true);
	makeTabMargin(output, 2);
	output.append("}\n");
	makeTabMargin(output, 2);
	output.append("return this;\n");
	makeTabMargin(output, 1);
	output.append("}\n");
	cfile.write(output, true);
}


