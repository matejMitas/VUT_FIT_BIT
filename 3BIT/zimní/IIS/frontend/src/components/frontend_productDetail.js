import React, {Component} from 'react';
import {Link, Redirect} from "react-router-dom";
import axios from 'axios';
//
import {API_BASE, ROUTE_PREFIX, TOKEN_NAME, on403, getUser, addToBasket} from '../const.js'
import Loader from './loader.js'

import placeholder from'../assets/black_12.jpg';

class ProductDetail extends Component {
    constructor(props) {
        super(props);
        // zjistime id caje z adresy

        this.state = {
            teaId : props.match.params.teaId,
            categoryName: props.match.params.categoryId,
            batchAmount: '',
            data: [],
            isLoading: false,
            error: null,
        };
    }

    async fetchData() {
        this.setState({ isLoading: true });
        try {
            const fetchedData = await axios(`${API_BASE}tea/${this.state.teaId}`, {
                timeout: 2000,
                headers: {
                    token: JSON.parse(localStorage.getItem(TOKEN_NAME))['token']
                }
            })
            this.setState({
                data: fetchedData.data,
                filteredData: fetchedData.data,
                isLoading: false
            })
        } catch (error) {
            this.setState({
                error: error.response.status,
                isLoading: false
            })
            if (error.response.status === 403) {
                on403()
            }
        }
    }

    componentDidMount() {
        //console.log(this.userInfo())
        this.setState({ isLoading: true });
        this.fetchData()
    }

    render() {
        const { data, categoryName, isLoading, error } = this.state
        const batches = data.vakra ? data.vakra : []
        
         if (isLoading) {
            return <Loader></Loader>
        }

        // nesedi kategorie a produkt
        if (data.kategorie && data.kategorie !== categoryName) {
            return <Redirect to={`${ROUTE_PREFIX}error`}/>
        }

        var ba
        if (getUser()['level'] < 3) {
            ba = batches.map(batch => {
                batch.teaId = this.state.data.jmeno_caje ? this.state.data.jmeno_caje : parseInt(this.props.match.params.teaId)
                    return (
                        <BatchSelector key={batch.varka_id} props={batch}></BatchSelector>
                    )
                }
            )
        } else {
            ba = batches.map(batch => {
                return (
                    <li key={batch.varka_id}>
                        <h4 className="productDetail__batches__price">{batch.cena} kč/5g</h4>
                        <h5 className="productDetail__batches__remaining">Zbývá: {batch.zbyvajici_mnozstvi} g</h5>
                    </li> 
                    )
                }
            )
        }

        return (
            <div>
                <nav className="subHeader__nav--small">
                    <div className="layout__inner">
                        <Link to={`${ROUTE_PREFIX}`}>Všechny čaje</Link>
                        <span className="subHeader__separator">/</span>
                        <Link to={`${ROUTE_PREFIX}caje/${data.kategorie}`}>{data.kategorie}</Link>
                    </div>
                </nav>
                <div className="subHeader subHeader__category layout__inner">
                    <h1>
                        {data.jmeno_caje}
                        <span className={this.state.categoryName}></span>
                    </h1>
                    <p>{data.charakter}</p>
                    <p>Kvalita <span>{data.kvalita}</span></p>

                    <h3>Várky</h3>

                    <ul className="productDetail__batches">
                        {ba}
                    </ul>
                </div>


            </div>
        )
    }
}


class BatchSelector extends Component {
    constructor(props) {
        super(props)
        this.state = {
            batchAmount: 0
        }
        //console.log(this.props)
    }

    handleValue = (event) => {
        this.setState({ batchAmount: event.target.value });
    }

    handleButton = async (batchId, event) => {

        //addToBasket(this.props.props.teaId, this.props.props.varka_id, this.state.batchAmount)

        try {
            var obj = {
                email: getUser()['email'],
                varka_id: this.props.props.varka_id,
                mnozstvi: this.state.batchAmount
            }

            //console.log(obj)

            var response = await axios({
                method: 'post',
                url: `${API_BASE}order/`,
                headers: {
                    token: getUser()['token']
                },
                data: obj
            })
            addToBasket(response.data.id, getUser()['email'])
            alert(`Přidáno do košíku, přesměrovat?`)
        } catch (error) {
            console.log(error.response)
            if (error.response.status === 400) {
                alert(`Nepodařilo se přidat do košíku - ${error.response.data}`)
            } else {
                on403()
            }
        }

        // if (this.state.batchAmount) {
        //     console.log('ano')
        // } else {
        //     console.log('ne')
        // }
        // console.log(this.state.batchAmount)
        // console.log(this.props.props.varka_id);
        this.setState({
            batchAmount: 0
        })
    }

    render() {
        var data = this.props.props

        return (
            <li>
                <h4 className="productDetail__batches__price">{data.cena} kč/5g</h4>
                <h5 className="productDetail__batches__remaining">Zbývá: {data.zbyvajici_mnozstvi} g</h5>
                <input 
                    className="productDetail__batches__amount" 
                    type="number" 
                    min="5" 
                    max={data.zbyvajici_mnozstvi} 
                    step="5" 
                    value={this.state.batchAmount}
                    onChange={this.handleValue}/>
                <button className="button__secondary" onClick={(event) => this.handleButton(data.varka_id, event)}>Přidat do košíku</button>
            </li>
        )
    }
}

//const UserInfo = withFetching(`http://localhost:5000/tea/83`)(UserInfoComp)
export default ProductDetail